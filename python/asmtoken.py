from typing import Optional

from ttype import TokenType
from error import Error
import re

TOKEN_SYMBOLS = ['[', ']', '.', ',', '-', '+', ':', '*']
TOKEN_REGISTER32 = ['eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'esp', 'ebp']
TOKEN_REGISTER16 = ['ax', 'bx', 'cx', 'dx', 'sp', 'bp', 'si', 'di']
TOKEN_REGISTER8 = ['al', 'cl', 'dl', 'bl', 'ah', 'ch', 'dh', 'bh']
TOKEN_SEGREG = ['es', 'cs', 'ss', 'ds', 'fs', 'gs']
TOKEN_DIRECTIVES = ['db', 'dw', 'dd']
TOKEN_INSTRUCTION = ['cld', 'dec', 'inc', 'cmp', 'shl', 'in', 'add', 'jnz', 'jmp', 'or']

TOKEN_DICT = [
    [TOKEN_SYMBOLS, TokenType.SYM],
    [TOKEN_REGISTER8, TokenType.REG8],
    [TOKEN_REGISTER16, TokenType.REG16],
    [TOKEN_REGISTER32, TokenType.REG32],
    [TOKEN_SEGREG, TokenType.SEGREG],
    [TOKEN_DIRECTIVES, TokenType.DIRECTIVE],
    [TOKEN_INSTRUCTION, TokenType.INSTRUCTION],
    [["segment"], TokenType.KEYWORD_SEGMENT],
    [["end"], TokenType.KEYWORD_END],
    [["ends"], TokenType.KEYWORD_ENDS],
]

numberHexRegex = re.compile(r"^-?[0-9a-f]+h$")
numberDecRegex = re.compile(r"^-?[0-9]+$")
numberBinRegex = re.compile(r"^-?[01]+b$")
identifierRegex = re.compile(r"^-?[a-z]\w*$")


def tokenTypeByValue(value: str) -> int:
    for d in TOKEN_DICT:
        for entry in d[0]:
            if entry == value:
                return d[1]

    if numberDecRegex.match(value):
        return TokenType.NUMBER_DEC
    elif numberBinRegex.match(value):
        return TokenType.NUMBER_BIN
    elif numberHexRegex.match(value):
        return TokenType.NUMBER_HEX
    elif identifierRegex.match(value):
        return TokenType.IDENTIFIER

    return -1


class ASMToken:
    def __init__(self, string_value: str, lexeme: 'ASMLexeme', type: int, line: int, char: int):
        self.string_value: str = string_value
        self.type: int = type
        self.file: str = lexeme.program.file_name
        self.lexeme: 'ASMLexeme' = lexeme
        self.line: int = line
        self.char: int = char

    @staticmethod
    def create(string_value: str, lexeme: 'ASMLexeme', line: int, char: int) -> (Optional[Error], 'ASMToken'):
        type = tokenTypeByValue(string_value)
        token = ASMToken(string_value, lexeme, type, line, char)
        if type == -1:
            return (Error("Unknown token type", token), None)

        return (None, token)

    def __str__(self):
        return "{} ({}) in {} at {}:{}".format(
            TokenType.to_string_value(self.type),
            self.string_value,
            self.file,
            self.line,
            self.char
        )

    def get_num_value(self) -> int:
        if self.type == TokenType.NUMBER_HEX:
            return int(self.string_value[:-1], base=16)
        elif self.type == TokenType.NUMBER_DEC:
            return int(self.string_value)
        else:
            return int(self.string_value[:-1], base=2)

    def to_ded_style(self) -> str:
        string_type = TokenType.to_string_value(self.type)
        # We need just identifier for the first time
        if self.type == TokenType.USER_SEGMENT:
            string_type = TokenType.to_string_value(TokenType.IDENTIFIER)
        elif self.type == TokenType.LABEL:
            string_type = TokenType.to_string_value(TokenType.IDENTIFIER)

        return "({:9} : {:11} : {})".format(
            "\"{}\"".format(self.string_value),
            string_type,
            len(self.string_value)
        )
        pass
