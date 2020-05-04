local first_pass = {}

local types = require("types")
local error = require("error")
local asm_storage = require("asmstorage")
local common = require("common")

local pretty = require("pl.pretty")
local math = require("math")

local global_fn = ""
local global_storage = {}

--
-- Redeclare compiler and source errors for current file
--
local function comp_error(msg) 
    error.comp_error("et1.lua", msg) 
end
local function source_error(line, char, msg) 
    error.error(global_fn, line, char, msg)
    table.insert(global_storage.error_lines, line) 
end

function first_pass.print_user_names(storage)
    print("|    Name    |    Type    | Line |    Value   |")
    for k,v in pairs(storage.user_names) do
        local type = "SEGMENT"
        local value = "---"

        local segment = storage:get_segment_by_line(v.start_line)
        if segment ~= nil then
            value = segment.name
        end

        if v.type == types.ut_label then
            type = "LABEL"
            local offset = storage.offsets[v.start_line]
            if offset == nil then
                offset = "00"
            end
            value = value .. ":" .. common.padleft(offset[2], 4, '0')
            
        elseif v.type == types.ut_var then
            type = "VAR "
            local offset = storage.offsets[v.start_line]
            if offset == nil then
                offset = "00"
            end

            if v.sub_type == types.ut_vt_db then
                type = type .. "DB"
            elseif v.sub_type == types.ut_vt_dd then
                type = type .. "DD"
            else
                type = type .. "DW"
            end

            value = value .. ":" .. common.padleft(offset[2], 4, '0')
        end

        print(string.format("| %10s | %10s | %4i | %10s |", v.name, type, v.start_line, value))
    end
end

function first_pass.print_segments(storage)
    print("|    Name    | Bt | Line |  Size |")
    for k,v in pairs(storage.user_names) do
        if v.type == types.ut_segment then
            print(string.format("| %10s | 32 | %4i |  %s |",
                v.name, v.start_line, common.padleft(v.size, 4, '0')))
        end
    end
end

function first_pass.print_segment_dest(storage)
    print("| Register | Destination |")
    for k, v in pairs(storage.segment_destinations) do
        local value = "NOTHING"
        if v.name ~= nil then 
            value = v.name
        end
        print(string.format("| %8s | %11s |", k, value))
    end
end

local function get_lexeme_type(tokens, token_types)
    assert(type(tokens) == "table")
    assert(type(token_types) == "table")

    if #tokens == 2 and token_types[1] == types.tt_identifier and tokens[2] == "SEGMENT" then
        return types.lt_segment_def;
    end;

    if #tokens == 3 and
        token_types[1] == types.tt_identifier and 
        (tokens[2] == "DB" or tokens[2] == "DW" or tokens[2] == "DD") and 
        (token_types[3] == types.tt_number2 or token_types[3] == types.tt_number10 or token_types[3] == types.tt_number16 or token_types[3] == types.tt_string) then
        return types.lt_var_def;
    end

    if #tokens == 2 and token_types[1] == types.tt_identifier and tokens[2] == "ENDS" then
        return types.lt_segment_end;
    end;

    if #tokens == 1 and tokens[1] == "END" then
        return types.lt_end;
    end;

    if #tokens > 2 and tokens[1] == "ASSUME" then
        return types.lt_assume;
    end;

    if #tokens >= 1 and token_types[1] == types.tt_instruction then
        return types.lt_instruction;
    end;

    if #tokens == 0 then
        return types.lt_blank
    end;

    if #tokens >= 2 and token_types[1] == types.tt_identifier and tokens[2] == ':' then
        return types.lt_label
    end;


    return types.lt_wrong 
end

local function token_to_number(token, token_type)
    if token_type == types.tt_number2 then
        return tonumber(token:sub(1, #token - 1), 2)
    elseif token_type == types.tt_number10 then
        return tonumber(token, 10)
    else
        return tonumber(token:sub(1, #token - 1), 16)
    end
end

local function get_info(tokens, structure, token_types, i, storage) 
    assert(type(tokens) == "table")
    assert(type(structure) == "table")
    assert(type(token_types) == "table")
    assert(type(i) == "number")
    assert(type(storage) == "table")

    local info = asm_storage.create_instruction_info(tokens[1])
    info.op_count = structure.op_count

    for i = 1, structure.op_count do 
        
        local op_info = asm_storage.create_op_info()
        local index = 0
        local len = 0
        
        if i == 1 then 
            info.op1 = op_info
            index = structure.operand1.index
            len = structure.operand1.len
        else 
            info.op2 = op_info
            index = structure.operand2.index
            len = structure.operand2.len
        end

        if len == 1 then
            
            op_info.token = tokens[index]
            op_info.token_type = token_types[index]
            
            if op_info.token_type == types.tt_reg8 then 
                op_info.type = types.ot_reg_small

            elseif op_info.token_type == types.tt_reg16 then 

                op_info.type = types.ot_reg_big
                if info.b16mode ~= nil and not info.b16mode then
                    source_error(i, 1, "Expected 32bit operand")
                    return;
                end

                info.b16mode = true
                
            elseif op_info.token_type == types.tt_reg32 then 
                
                op_info.type = types.ot_reg_big
                if info.b16mode ~= nil and info.b16mode then
                    source_error(i, 1, "Expected 16bit operand")
                    return;
                end

                info.b16mode = false
            
            elseif op_info.token_type == types.tt_number2 or 
                   op_info.token_type == types.tt_number10 or
                   op_info.token_type == types.tt_number16 then 
            
                -- Imm
            
                op_info.imm = token_to_number(op_info.token, op_info.token_type)
                if math.abs(op_info.imm) < 255 then 
                    op_info.type = types.ot_imm_small
                else
                    op_info.type = types.ot_imm_big
                    if info.b16mode ~= nil and info.b16mode then 
                        if op_info.imm > 0xFFFF then
                            source_error(i, 1, "Constant is too big for 16bit mode")
                            return;
                        end
                    else
                        if op_info.imm > 0xFFFFFF then
                            source_error(i, 1, "Constant is too big for 32bit mode")
                            return;
                        end
                    end
                end
            else
                -- Label
                local un = storage:get_user_name(types.ut_label, op_info.token)
                if un == nil then
                    op_info.type = types.ot_label_f
                else
                    op_info.type = types.ot_label_b
                end
            end
        else
            if len < 5 or len > 9 then
                source_error(i, 1, "Invalid memory format: Expected from 5 to 9 tokens")
                return;
            end

            local offset = 0

            if token_types[index] == types.tt_byte then
                offset = 2
                op_info.type = types.ot_mem_small

                if tokens[index + 1] ~= "PTR" then 
                    source_error(i, 1, "Expected PTR keyword")
                    return;
                end

            elseif token_types[index] == types.tt_word then
                offset = 2
                if info.b16mode ~= nil and not info.b16mode then
                    source_error(i, 1, "Expected 32bit operand")
                    return;
                end
                if tokens[index + 1] ~= "PTR" then 
                    source_error(i, 1, "Expected PTR keyword")
                    return;
                end

                op_info.type = types.ot_mem_big
                info.b16mode = true

            elseif token_types[index] == types.tt_dword then
                offset = 2

                if info.b16mode ~= nil and info.b16mode then
                    source_error(i, 1, "Expected 16bit operand")
                    return;
                end
                if tokens[index + 1] ~= "PTR" then 
                    source_error(i, 1, "Expected PTR keyword")
                    return;
                end

                op_info.type = types.ot_mem_big
                info.b16mode = false
            else 
                op_info.type = types.ot_mem_undef
            end
            
            if token_types[index + offset] == types.tt_segment_reg then
                if tokens[index + offset + 1] ~= ":" then 
                    source_error(i, 1, "Expected \":\" symbol")
                    return;
                end
                info.seg_reg = tokens[index + offset]
                offset = offset + 2
            end

            if len - offset ~= 5 then
                source_error(i, 1, "Invalid memory format: Invalid tokens count")
                return;
            end

            if  tokens[index + offset + 0] ~= '[' or
                tokens[index + offset + 2] ~= '+' or 
                tokens[index + offset + 4] ~= ']' then
                source_error(i, 1, "Invalid memory format")
                return;
            end

            op_info.sum1 = tokens[index + offset + 1]
            op_info.sum1_type = token_types[index + offset + 1]
            op_info.sum2 = tokens[index + offset + 3]
            op_info.sum2_type = token_types[index + offset + 3]

            if op_info.sum1_type ~= op_info.sum2_type then
                source_error(i, 1, "Invalid memory format: Index registers mismatch")
                return;
            end

            if op_info.sum1_type ~= types.tt_reg16 and  op_info.sum1_type ~= types.tt_reg32 then
                source_error(i, 1, "Invalid memory format: Wrong index registers type")
                return;
            end
        end
    end
    
    -- Check for unset b16 mode
    if info.b16mode == nil then
        info.b16mode = false
    end
    
    -- Check for udef memory
    if info.op1.type == types.ot_mem_undef then
        if info.op_count == 1 then 
            source_error(i, 1, "Invalid memory format: Undefined memory bit depth")
            return;
        end

        if info.op2.type == types.ot_imm_small or info.op2.type == types.ot_imm_big or info.op2.type == types.ot_mem_undef then
            source_error(i, 1, "Invalid memory format: Undefined memory bit depth")
            return;
        end

        if info.op2.type == types.ot_reg_small then
           info.op1.type = types.ot_mem_small 
        else
            info.op1.type = types.ot_mem_big 
        end

    elseif info.op2.type == types.ot_mem_undef then
        if info.op_count == 1 then 
            source_error(i, 1, "Invalid memory format: Undefined memory bit depth")
            return;
        end

        if info.op1.type == types.ot_imm_small or info.op1.type == types.ot_imm_big or info.op1.type == types.ot_mem_undef then
            source_error(i, 1, "Invalid memory format: Undefined memory bit depth")
            return;
        end

        if info.op1.type == types.ot_reg_small then
            info.op2.type = types.ot_mem_small 
         else
             info.op2.type = types.ot_mem_big 
         end
    end
    return info
end

local instructions = {}

local function push_instruction(name, opcode, op_count, op1, op2, modrm, imm, reg_opcode, packed_reg)
    local instr = {}
    instr.name = name
    instr.opcode = opcode
    instr.op_count = op_count
    instr.op1 = op1
    instr.op2 = op2
    instr.modrm = modrm
    instr.imm = imm
    instr.reg_opcode = reg_opcode
    instr.packed_reg = packed_reg
    table.insert(instructions, instr)
end

-- F9   STC

-- 50+rw PUSH r32

-- F6 /5 Imul r/m8
-- F7 /5 Imul r/m32

-- 8A /r MOV r8, r/m8
-- 8B /r MOV r32, r/m32

-- 12 /r Adc r8, r/m8
-- 13 /r Adc r32, r/m32

-- 28 /r SUB r/m8, r8
-- 29 /r SUB r/m32, r32

-- 80 /7 ib CMP r/m8, imm8
-- 81 /7 id CMP r/m32, imm32
-- 83 /7 ib CMP r/m32, imm8

-- 80 /6 ib XOR r/m8, imm8
-- 81 /6 ib XOR r/m32, imm32
-- 83 /6 ib XOR r/m32, imm8

-- 72 cb JC rel8
-- OF 82 cw JC rel16

-- EB cb JMP rel8
-- E9 cw JMP rel16

function first_pass.register_instructions()
    instructions = {}
    --        name, opcode, op_count, op1, op2,modrm,imm,reg_opcode,packed_reg
    push_instruction("STC",  0xF9, 0)
    push_instruction("PUSH", 0x50, 1, types.ot_reg_big, nil, nil, nil, nil, true)
    
    push_instruction("IMUL", 0xF6, 1, types.ot_mem_small, nil, true, nil, 5, nil)
    push_instruction("IMUL", 0xF7, 1, types.ot_mem_big, nil, true, nil, 5, nil)

    push_instruction("MOV",  0x8A, 2, types.ot_reg_small, types.ot_reg_small, true, nil, nil, nil)
    push_instruction("MOV",  0x8B, 2, types.ot_reg_big, types.ot_reg_big, true, nil, nil, nil)

    push_instruction("ADC",  0x12, 2, types.ot_reg_small, types.ot_mem_small, true, nil, nil, nil)
    push_instruction("ADC",  0x13, 2, types.ot_reg_big, types.ot_mem_big, true, nil, nil, nil)

    push_instruction("SUB",  0x28, 2, types.ot_mem_small, types.ot_reg_small, true, nil, nil, nil)
    push_instruction("SUB",  0x29, 2, types.ot_mem_big, types.ot_reg_big, true, nil, nil, nil)

    push_instruction("CMP",  0x80, 2, types.ot_reg_small, types.ot_imm_small, true, 1, 7, nil)
    push_instruction("CMP",  0x83, 2, types.ot_reg_big, types.ot_imm_small, true, 1, 7, nil)
    push_instruction("CMP",  0x81, 2, types.ot_reg_big, types.ot_imm_big, true, 4, 7, nil)
    
    push_instruction("XOR",  0x80, 2, types.ot_mem_small, types.ot_imm_small, true, 1, 6, nil)
    push_instruction("XOR",  0x81, 2, types.ot_mem_big, types.ot_imm_big, true, 4, 6, nil)
   
    push_instruction("JC",   0x00, 1, types.ot_label_f)
    push_instruction("JC",   0x00, 1, types.ot_label_b)

    push_instruction("JMP",  0x00, 1, types.ot_label_f)
    push_instruction("JMP",  0x00, 1, types.ot_label_b)
end

local function assign_instruction(storage, instruction_info, tokens, token_types, i)
    local last_error = nil
    for k,v in pairs(instructions) do
        local wrong = false
        if v.name ~= tokens[1] then 
            wrong = true
        end 

        if not wrong then
            if v.op_count == instruction_info.op_count then
                if (v.op_count == 1 or v.op_count == 2) and v.op1 ~= instruction_info.op1.type then
                    last_error = "Couldn't match instruction: First operand type mismatch"
                    wrong = true
                end

                if not wrong and (v.op_count == 2) and v.op2 ~= instruction_info.op2.type then
                    if v.op2 == types.ot_imm_big and instruction_info.op2.type == types.ot_imm_small then
                        instruction_info.op2.type = types.ot_imm_big
                    else
                        last_error = "Couldn't match instruction: Second operand type mismatch"
                        wrong = true
                    end
                end

                if not wrong then
                    return v
                end
            else
                source_error(i, 1, "Couldn't match instruction: Invalid operands count")
                return; 
            end
        end
    end

    if last_error ~= nil then
        source_error(i, 1, last_error)
    else
         source_error(i, 1, "Couldn't match instruction: Unknown name")
    end
    return;
end

local function get_instruction_size(storage, instruction_info, tokens, token_types, i)
    local size = 0
    local debug = false
    local prot = instruction_info.instr_prot

    if tokens[1] == "JMP" or tokens[1] == "JC" then
        local un = storage:get_user_name(types.ut_label, tokens[2])
        if un == nil then
            if tokens[1] == "JMP" then return 5
            else return 6
            end
        end

        local current_seg = storage:get_segment_by_line(i)

        local destination = storage.offsets[un.start_line][1]
        local source = current_seg.size

        local near = math.abs(destination - source) < 127

        if tokens[1] == "JMP" then
            if near then return 2
            else return 5 end
        else
            if near then return 2
            else return 6 end
        end
    end

    size = 1
    if debug then print('opcode') end

    if prot.modrm ~= nil then
        size = size + 1
        if debug then print('modrm') end
    end

    if prot.imm ~= nil then
        if prot.imm == 1 then 
            size = size + 1
            if debug then print('imm8') end
        elseif instruction_info.b16mode then 
            size = size + 2
            if debug then print('imm16') end
        else
            size = size + 4
            if debug then print('imm32') end
        end
    end

    local op_infos = {}
    if instruction_info.op_count == 1 then 
        op_infos = { instruction_info.op1 }
    elseif instruction_info.op_count == 2 then 
        op_infos = { instruction_info.op1, instruction_info.op2 }
    end

    if instruction_info.seg_reg ~= nil then 
        size = size + 1
        if debug then print('prefix_seg') end
    end

    if instruction_info.b16mode then
        size = size + 1
        if debug then print('prefix_data') end
    end

    for k,v in pairs(op_infos) do
        if v.type == types.ot_mem_small or v.type == types.ot_mem_big then
            if v.sum1_type == types.tt_reg32 then
                size = size + 1
                if debug then print('sib') end
            else
                size = size + 1
                if debug then print('prefix_index') end
            end
        end        
    end

    return size
end

local function get_data_size(storage, tokens, token_types, i)
    if tokens[2] == "DD" then return 4; end
    if tokens[2] == "DW" then return 2; end

    if token_types[3] == types.tt_string then 
        return #tokens[3];
    end

    return 1;
end

function first_pass.do_first_pass(fn, i, line, tokens, token_types, structure, storage, et2_print)
    assert(type(fn) == "string")
    assert(type(i) == "number")
    assert(type(line) == "string")
    assert(type(tokens) == "table")
    assert(type(token_types) == "table")
    assert(type(structure) == "table")
    assert(type(storage) == "table")
    assert(type(et2_print) == "boolean")

    global_fn = fn
    global_storage = storage

    -- determine lexeme type
    local type = get_lexeme_type(tokens, token_types)
    if type == types.lt_wrong then 
        source_error(i, 1, "Unknown instruction type")
        return;
    end;

    -- proceed segments, labels, assume, variables
    if type == types.lt_segment_def then
        local segment = storage:get_user_name(types.ut_segment, tokens[1])
        if segment ~= nil then
            source_error(i, 1, "Segment redefinition. Already defined at line" .. segment.start_line)
            return;
        end

        segment = asm_storage.create_user_name(types.ut_segment, tokens[1])
        segment.start_line = i
        storage:push_user_name(segment)

    elseif type == types.lt_segment_end then
        
        local segment = storage:get_user_name(types.ut_segment, tokens[1])
        if segment == nil then
            source_error(i, 1, "Unable to find matching segment")
            return;
        end

        if segment.end_line ~= 0 then
            source_error(i, 1, "Segment already closed")
            return;
        end

        segment.end_line = i

    elseif type == types.lt_var_def then

        local variable = storage:get_user_name(types.ut_var, tokens[1])
        if variable ~= nil then
            source_error(i, 1, "Variable redefinition. Already defined at line " .. variable.start_line)
            return;
        end

        variable = asm_storage.create_user_name(types.ut_var, tokens[1])
        variable.start_line = i

        if token_types[3] == types.tt_string then
            if tokens[2] ~= "DB" then
                source_error(i, 1, "Expected \"DB\" directive")
                return;
            end
        else 
            local imm = token_to_number(tokens[3], token_types[3])
            local too_big = false
            if tokens[2] == "DB" then
                if imm > 0xFF then too_big = true; end
            elseif tokens[2] == "DW" then
                if imm > 0xFFFF then too_big = true; end
            else
                if imm > 0xFFFFFF then too_big = true; end
            end

            if too_big then
                source_error(i, 1, "Constant is too big")
                return;
            end
        end    
        
        if tokens[2] == "DB" then
            variable.sub_type = types.ut_vt_db
        elseif tokens[2] == "DW" then
            variable.sub_type = types.ut_vt_dw
        else
            variable.sub_type = types.ut_vt_dd
        end

        storage:push_user_name(variable)

        local current_seg = storage:get_segment_by_line(i)

        local size = get_data_size(storage, tokens, token_types, i) 
        
        storage.offsets[i] = { size, current_seg.size } 
        current_seg.size = current_seg.size + size

    elseif type == types.lt_label then

        local label = storage:get_user_name(types.ut_label, tokens[1])
        if label ~= nil then
            source_error(i, 1, "Label redefinition. Already defined at line " .. label.start_line)
            return;
        end

        label = asm_storage.create_user_name(types.ut_label, tokens[1])
        label.start_line = i
        storage:push_user_name(label)

        local current_seg = storage:get_segment_by_line(i)
        storage.offsets[i] = { 0, current_seg.size } 
        current_seg.size = current_seg.size

    elseif type == types.lt_assume then

        if token_types[2] ~= types.tt_segment_reg or
           tokens     [3] ~= ":" or
           token_types[4] ~= types.tt_identifier or
           tokens     [5] ~= "," or
           token_types[6] ~= types.tt_segment_reg  or
           tokens     [7] ~= ":"  or
           token_types[8] ~= types.tt_identifier then

            source_error(i, 1, "Wrong ASSUME format")
            return;
        end

        local reg1 = tokens[2]
        local seg1 = tokens[4]
   
        local segment = storage:get_user_name(types.ut_segment, seg1)
        if segment == nil then
            source_error(i, 1, "Unable to find matching segment")
            return;
        end

        storage.segment_destinations[reg1] = segment

        local reg2 = tokens[6]
        local seg2 = tokens[8]

        local segment = storage:get_user_name(types.ut_segment, seg2)
        if segment == nil then
            source_error(i, 1, "Unable to find matching segment")
            return;
        end

        storage.segment_destinations[reg2] = segment

    elseif type == types.lt_instruction then

        storage.instruction_infos[i] = get_info(tokens, structure, token_types, i, storage)
        
        if storage.instruction_infos[i] == nil then
            return
        end

        storage.instruction_infos[i].instr_prot = assign_instruction(storage, storage.instruction_infos[i], tokens, token_types, i)
        if storage.instruction_infos[i].instr_prot == nil then
            return
        end
        
        local current_seg = storage:get_segment_by_line(i)

        local size = get_instruction_size(storage, storage.instruction_infos[i], tokens, token_types, i) 
        
        storage.offsets[i] = { size, current_seg.size } 
        current_seg.size = current_seg.size + size
    end
end

return first_pass