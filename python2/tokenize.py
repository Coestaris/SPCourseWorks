import re

directives = [".DATA", ".CODE", ".MODEL", "END", "DB", "DD"]
instructions = ["STD", "JNGE", "JMP", "PUSH", "POP", "IDIV", "ADD", "ADC", "IN", "OR"]
registers32 = ["EAX", "EBX", "ECX", "EDX", "EBP", "ESP", "ESI", "EDI"]
registers8 = ["AL", "BL", "DL", "CL", "AH", "BH", "DH", "CH"]
models = ["SMALL", "TINY"]
splitters = [".", ":", " ", "[", "]", "\n", "\t", ","]

dec_re = re.compile(r"[0-9]+")
bin_re = re.compile(r"[01]+B")
id_re = re.compile(r"[A-Za-z]\d{,3}")


class TokenType:
    directive = 1
    register32 = 2
    register8 = 3
    model = 4
    data_type = 5
    instructions = 6
    splitter = 7
    user_type = 8
    dec_number = 9
    bin_number = 10
    unknown = 11


def type_detector(string_token):
    if string_token in directives:
        return TokenType.directive
    elif string_token in registers32:
        return TokenType.register32
    if string_token in registers8:
        return TokenType.register8
    if string_token in models:
        return TokenType.model
    if string_token in instructions:
        return TokenType.instructions
    if string_token in splitters:
        return TokenType.splitter

    if re.fullmatch(dec_re, string_token):
        return TokenType.dec_number

    if re.fullmatch(bin_re, string_token):
        return TokenType.bin_number

    if re.fullmatch(id_re, string_token):
        return TokenType.user_type

    else:
        return TokenType.unknown


STRING_TYPES = {
    TokenType.directive: "directive",
    TokenType.register32: "register32",
    TokenType.register8: "register8",
    TokenType.model: "model",
    TokenType.data_type: "directive",
    TokenType.instructions: "instruction",
    TokenType.splitter: "splitter",
    TokenType.dec_number: "dec number",
    TokenType.bin_number: "bin number",
    TokenType.user_type: "user_defined",
    TokenType.unknown: "unknown"
}
