local types = {}

local common = require("common")

local directives = { "SEGMENT", "ENDS", "ASSUME", "END", "DB", "DW", "DD", "DB" }
local instructions = { "STC", "PUSH", "MOV", "ADC", "SUB", "XOR", "JMP", "JC", "CMP", "IMUL" }
local registers_8 = { "AL", "BL", "DL", "CL", "AH", "CH", "DH", "BH" }
local registers_16 = { "AX", "BX", "DX", "CX", "SI", "DI", "SP", "BP" }
local registers_32 = { "EAX", "EBX", "EDX", "ECX", "ESI", "EDI", "ESP", "EBP" }
local registers_seg = { "CS", "DS", "ES", "FS", "GS", "SS" }
local symbols = { ":", ",", "+", "-", "[", "]" }

types.tt_directive = 0
types.tt_instruction = 1
types.tt_identifier = 2
types.tt_symbol = 3
types.tt_reg8 = 4 
types.tt_reg16 = 5 
types.tt_reg32 = 6 
types.tt_number16 = 7 
types.tt_number10 = 8 
types.tt_number2 = 9
types.tt_string = 10 
types.tt_dword = 11
types.tt_byte = 12 
types.tt_word = 13
types.tt_ptr = 14
types.tt_segment_reg = 15 

local tt_to_name = {
    [types.tt_directive] = "DIRECTIVE",
    [types.tt_instruction] = "INSTRUCTION",
    [types.tt_identifier] = "IDENTIFIER",
    [types.tt_symbol] = "SYMBOL",
    [types.tt_reg8] = "REG8",
    [types.tt_reg16] = "REG16",
    [types.tt_reg32] = "REG32",
    [types.tt_number2] = "NUMBER2",
    [types.tt_number10] = "NUMBER10",
    [types.tt_number16] = "NUMBER16",
    [types.tt_string] = "STRING",
    [types.tt_dword] = "DWORD",
    [types.tt_byte] = "BYTE",
    [types.tt_word] = "WORD",
    [types.tt_ptr] = "PTR",
    [types.tt_segment_reg] = "REG_SEG",
}

local number16_re = "-?0[0-9A-F]+H"
local number10_re = "-?[0-9]+"
local number2_re = "-?0[01]+B"
local id_re = "[A-Z][A-Z0-9_]+"

--
--
local function is_number_hex(token)
    return token:match(number16_re) == token
end

--
--
--
local function is_number_bin(token)
    return token:match(number2_re) == token
end

--
--
--
local function is_number_dec(token)
    return token:match(number10_re) == token
end

--
--
--
local function is_identifier(token)
    return token:match(id_re) == token
end

--
--
--
function types.get_name(token_type)
    assert(type(token_type) == "number")
    return tt_to_name[token_type]
end

--
--
--
function types.get_token_type(token)
    assert(type(token) == "string")
    
    if common.has_value(directives, token) then
        return types.tt_directive
    elseif common.has_value(instructions, token) then
        return types.tt_instruction
    elseif common.has_value(registers_8, token) then
        return types.tt_reg8
    elseif common.has_value(registers_16, token) then
        return types.tt_reg16
    elseif common.has_value(registers_32, token) then
        return types.tt_reg32
    elseif common.has_value(registers_seg, token) then
        return types.tt_segment_reg
    elseif common.has_value(symbols, token) then
        return types.tt_symbol
    elseif token == "BYTE" then
        return types.tt_byte
    elseif token == "WORD" then
        return types.tt_word
    elseif token == "DWORD" then
        return types.tt_dword
    elseif token == "PTR" then
        return types.tt_ptr
    else 
        if is_number_hex(token) then
            return types.tt_number16
        elseif is_number_dec(token) then
            return types.tt_number10
        elseif is_number_bin(token) then
            return types.tt_number2
        elseif is_identifier(token) then
            return types.tt_identifier
        elseif token:sub(1, 1) == "\"" then
            return types.tt_string
        else
            return -1
        end
    end
end

return types