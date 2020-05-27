local second_pass = {}

local asmstorage = require("asmstorage")
local types = require("types")
local first_pass = require("first_pass")

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

local function set_data_bytes(bytes, tokens, token_types, structure, storage)
    local num = first_pass.token_to_number(tokens[3], token_types[3])
    if tokens[2] == "DB" then 
        if token_types[3] == types.tt_string then
            bytes:set_imm_long(tokens[3])
        else
            bytes:set_imm8(num)
        end
    elseif tokens[2] == "DW" then 
        bytes:set_imm16(num)
    else 
        bytes:set_imm32(num)
    end
end

local function set_code_bytes(bytes, i, tokens, token_types, structure, storage)
    local info = storage.instruction_infos[i]
    local prot = storage.instruction_infos[i].instr_prot
    bytes:set_opcode(prot.opcode)
    if prot.packed_reg then bytes:pack_reg(info.op1.token) end
    if info.b16mode then bytes:set_data_prefix() end
    if prot.modrm then
        local ops = {info.op1, info.op2}
        if prot.op_count == 1 then ops = { ops[1] } end
        local mop = {}
        local mnop = {}

        if prot.modrm == 1 then 
            mop = ops[1] 
            nmop = ops[2]
        else 
            mop = ops[2] 
            nmop = ops[1]
        end

        if nmop ~= nil then
            if nmop.type == types.ot_reg_small or nmop.type == types.ot_reg_big then
                bytes:set_reg_reg(nmop.token)
            end
        else
            if prot.reg_opcode ~= nil then
                bytes:set_reg_const(prot.reg_opcode)
            else
                print("prot.reg_opcode is nil =c")
            end
        end

        if mop.type == types.ot_reg_small or mop.type == types.ot_reg_big then
            bytes:set_mod_reg(mop.token)
        else
            bytes:set_mod_sum(mop.sum1_type, mop.sum1, mop.sum2)
        end
    end
end

function second_pass.do_second_pass(filename, i, line, tokens, token_types, structure, storage, lex_type)
    bytes = asmstorage.create_bytes()
    if lex_type == types.lt_instruction then
        set_code_bytes(bytes, i, tokens, token_types, structure, storage)
    elseif lex_type == types.lt_var_def then
        set_data_bytes(bytes, tokens, token_types, structure, storage)
    end
    return bytes
end

return second_pass