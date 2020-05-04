local tokenize = {}

local pretty = require("pl.pretty")

local types = require("types")
local first_pass = require("first_pass")
local common = require("common")
local asm_storage = require("asmstorage")
local error = require("error")

local split_characters = { "\t", " ", '\n', '[', ']', '.', ',', '*', '+', ':' } 

local global_fn = ""
local global_storage = {}

local function create_structure()
    structure = {}
    
    structure.name = -1
    structure.instruction = -1
    structure.op_count = 0
    structure.operand1 = {}
    structure.operand1.index = -1
    structure.operand1.len = -1
    structure.operand2 = {}
    structure.operand2.index = -1
    structure.operand2.len = -1

    function structure.has_name(self)
        return self.name ~= -1
    end

    function structure.has_instruction(self)
        return self.instruction ~= -1
    end

    function structure.has_operands(self)
        return self.operand1.index ~= -1
    end

    function structure.print(self)
        local operands = ""
        local instruction = ""
        local name = ""

        if self:has_name() then
            name = "1"
        else
            name = "--"
        end

        if self:has_instruction() then
            instruction = string.format("%d %d | ", self.instruction, 1)

            if self:has_operands() then
                operands = operands .. string.format("%d %d", 
                    self.operand1.index,
                    self.operand1.len)
                
                if self.operand2.index ~= -1 then
                    operands = operands .. string.format(" | %d %d", 
                        self.operand2.index,
                        self.operand2.len)
                end
            end
        end

        print(string.format("struct :| %s | %s %s", name, instruction, operands))
    end

    return structure
end

--
-- Redeclare compiler and source errors for current file
--
local function comp_error(msg) 
    error.comp_error("tokenize.lua", msg) 
end
local function source_error(line, char, msg) 
    error.error(global_fn, line, char, msg)
    table.insert(global_storage.error_lines, line) 
end

--
-- Reads all lines from source file
--
local function read_lines(filename)
    assert(type(filename) == "string")

    lines = {}
    for line in io.lines(filename) do 
        lines[#lines + 1] = line
    end
    return lines
end

--
--  
--
local function tokenize_line(line)
    assert(type(line) == "string")

    local tokens = {}
    local token = ""
    local string = false
    local skip_char = false
    for i = 1, #line do
        local char = line:sub(i, i)

        if char == "\"" then 
            string = not string
            
            if not string then
                tokens[#tokens + 1] = token
                skip_char = true
                token = ""
            end
        end

        if string then
            if skip_char then
                skip_char = false
            else
                token = token .. char
            end
        else
            if common.has_value(split_characters, char) then

                local trimmed = common.trim(token)
                if not common.is_empty(trimmed) then
                    tokens[#tokens + 1] = trimmed:upper()
                end
                
                local trimmed = common.trim(char)
                if not common.is_empty(trimmed) then
                    if skip_char then
                        skip_char = false
                    else
                        tokens[#tokens + 1] = trimmed:upper()
                    end
                end

                token = ""
            else
                if skip_char then
                    skip_char = false
                else
                    token = token .. char 
                end
            end
        end
    end

    local trimmed = common.trim(token)
    if not common.is_empty(trimmed) then
        tokens[#tokens + 1] = trimmed:upper()
    end
    return tokens
end

--
--
--
local function proceed_line(line, line_id, storage, et1_print)
    assert(type(line) == "string")
    assert(type(storage) == "table")
    assert(type(et1_print) == "boolean")

    local tokens = tokenize_line(line)
    local token_types = {}

    local output = ""

    for i, v in ipairs(tokens) do
        local type = types.get_token_type(v)

        if type == -1 then
            source_error(line_id, i, string.format("Unknown token type of token %q", v))
            return token_types, tokens
        else
            output = output .. string.format("(%q : %s : %i)", v, types.get_name(type), i) .. " | "
        end

        token_types[#token_types + 1] = type
    end
     
    if et1_print then
        print("tokens :| " .. output)
    end
    
    return token_types, tokens
end

local function get_structure(token_types, tokens, i)
    assert(type(token_types) == "table")
    assert(type(tokens) == "table")

    local structure = create_structure()
    
    if #token_types == 0 then
        return structure
    end

    local counter = 1

    if token_types[1] == types.tt_identifier then
        structure.name = 1
        counter = 2
        if #tokens == 2 and tokens[2] == ":" then
            counter = 3
        end    
    end

    if #token_types == counter - 1 then
        if counter == 2 then 
            source_error(i, counter, 'Name without instruction')
        end;
        return structure
    end

    if token_types[counter] == types.tt_directive or token_types[counter] == types.tt_instruction then
        structure.instruction = counter
        counter = counter + 1
    else 
        source_error(i, counter, 'Instruction expected')
        return structure
    end;

    if #token_types == counter - 1 then
        return structure
    end

    local coma_index = -1
    for i = counter, #token_types do
        if tokens[i] == "," then 
            coma_index = i
            break
        end
    end

    structure.operand1.index = counter
    if coma_index == -1 then
        structure.operand1.len = #tokens - counter + 1
        structure.op_count = 1
    else
        structure.operand1.len = coma_index - counter 
        structure.operand2.index = coma_index + 1
        structure.operand2.len = #tokens - coma_index
        structure.op_count = 2
    end

    return structure
end

--
-- Main ET1 Proceed routine. Tokenize and append types
--
function tokenize.proceed(filename, storage, et1_print, next_et_function, et2_print)
    assert(type(filename) == "string")
    assert(type(storage) == "table")
    assert(type(et1_print) == "boolean")
    assert(type(next_et_function) == "function")
    assert(type(et2_print) == "boolean")

    global_fn = filename
    global_storage = storage

    local lines = read_lines(filename)
    for i, line in pairs(lines) do

        if et1_print then
            print(string.format("line#%2i:| %s", i, line))
        end
        
        local token_types, tokens = proceed_line(line, i, storage, et1_print)

        local structure = {}
        if not storage:has_error(i) then
            structure = get_structure(token_types, tokens, i)
        end;

        if et1_print and not storage:has_error(i) then
            structure:print()
            print()
        end
        
        if not storage:has_error(i) then
            next_et_function(filename, i, line, tokens, token_types, structure, storage, et2_print)
        end        
        
        if et2_print then
            if storage.offsets[i] ~= nil then
                print(string.format("%2i:| %s | %s | %s", i, 
                    common.padleft(storage.offsets[i][2], 4, '0'), 
                    common.padleft(storage.offsets[i][1], 4, '0'), line))
            else
                print(string.format("%2i:|  --  |  --  | %s", i, line))
            end
        end
    end

    if et2_print then

        print()
        print("User defined names")
        first_pass.print_user_names(storage)
        
        print()
        print("User defined segments")
        first_pass.print_segments(storage)

        print()
        print("Segments destinations")
        first_pass.print_segment_dest(storage)
    end
end

return tokenize