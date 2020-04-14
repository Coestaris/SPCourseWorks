local first_pass = {}

local pretty = require("pl.pretty")

local common = require("common")
local error = require("error")

local split_characters = { "\t", " ", '\n', '[', ']', '.', ',', '*', '+', ':' } 

local directives = { "SEGMENT", "ENDS", "ASSUME", "END", "DB", "DW", "DD", "DB" }
local instructions = { "STC", "PUSH", "MOV", "ADC", "SUB", "XOR", "JMP", "JC", "CMP" }
local registers_8 = { "AL", "BL", "DL", "CL", "AH", "CH", "DH", "BH" }
local registers_16 = { "AX", "BX", "DX", "CX", "SI", "DI", "SP", "BP" }
local registers_32 = { "EAX", "EBX", "EDX", "ECX", "ESI", "EDI", "ESP", "EBP" }
local registers_seg = { "CS", "DS", "ES", "FS", "GS", "SS" }
local symbols = { ":", ",", "+", "-", "[", "]" }

local tt_directive = 0
local tt_instruction = 1
local tt_identifier = 2
local tt_symbol = 3
local tt_reg8 = 4 
local tt_reg16 = 5 
local tt_reg32 = 6 
local tt_number16 = 7 
local tt_number10 = 8 
local tt_number2 = 9
local tt_string = 10 
local tt_dword = 11
local tt_byte = 12 
local tt_word = 13
local tt_ptr = 14
local tt_segment_reg = 15 

local tt_to_name = {
    [tt_directive] = "DIRECTIVE",
    [tt_instruction] = "INSTRUCTION",
    [tt_identifier] = "IDENTIFIER",
    [tt_symbol] = "SYMBOL",
    [tt_reg8] = "REG8",
    [tt_reg16] = "REG16",
    [tt_reg32] = "REG32",
    [tt_number2] = "NUMBER2",
    [tt_number10] = "NUMBER10",
    [tt_number16] = "NUMBER16",
    [tt_string] = "STRING",
    [tt_dword] = "DWORD",
    [tt_byte] = "BYTE",
    [tt_word] = "WORD",
    [tt_ptr] = "PTR",
    [tt_segment_reg] = "REG_SEG",
}

local number16_re = "-?[0-9]+h"
local number10_re = "^-?[0-9]+$"
local number2_re = "^-?[01]+b$"
local id_re = "^-?[a-z]\\w*$"


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

local function is_number_hex(token)
    local match = token:match(number16_re)
    print(match)
    return match == token
end

local function is_number_bin(token)
    return false
end

local function is_number_dec(token)
    return false
end

local function is_identifier(token)
    return false
end

--
--
--
local function get_token_type(token)
    assert(type(token) == "string")
    
    if common.has_value(directives, token) then
        return tt_directive
    elseif common.has_value(instructions, token) then
        return tt_instruction
    elseif common.has_value(registers_8, token) then
        return tt_reg8
    elseif common.has_value(registers_16, token) then
        return tt_reg16
    elseif common.has_value(registers_32, token) then
        return tt_reg32
    elseif common.has_value(registers_seg, token) then
        return tt_segment_reg
    elseif common.has_value(symbols, token) then
        return tt_symbol
    else 
        if is_number_hex(token) then
            return tt_number16
        elseif is_number_dec(token) then
            return tt_number10
        elseif is_number_bin(token) then
            return tt_number2
        elseif is_identifier(token) then
            return tt_identifier
        else
            return -1
        end
    end
end

--
--
--
local function proceed_line(line, line_id, storage)
    assert(type(line) == "string")
    assert(type(storage) == "table")

    local tokens = tokenize_line(line)
    local types = {}

    local output = ""

    for i, v in ipairs(tokens) do
        types[i] = get_token_type(v)
        if types[i] == -1 then
            source_error(line_id, i, string.format("Unknown token type of token %q", v))
            return
        else
            output = output .. string.format("(%q : %s)", v, tt_to_name[types[i]]) .. " | "
        end
    end
     
    print("tokens: " .. output)
end

--
-- Main ET1 Proceed routine. Tokenize and append types
--
function first_pass.proceed(filename, storage, et1_print)
    assert(type(filename) == "string")
    assert(type(storage) == "table")
    assert(type(et1_print) == "boolean")

    global_fn = filename
    global_storage = storage

    local lines = read_lines(filename)
    for i, line in pairs(lines) do
        print("source :" .. line)
        proceed_line(line, i, storage)
        print()
    end
    
end

--
-- Main ET1 Output routine. Prints tokens and sentences
--
function first_pass.output(storage)
    assert(type(storage) == "table")
    print("Et1 table")
end

return first_pass