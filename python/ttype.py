tokenMap = [
    "SYM",
    "REG8",
    "REG16",
    "REG32",
    "SEGREG",
    "DIRECTIVE",
    "INSTRUCTION",
    "KEYWORD_SEGMENT",
    "KEYWORD_END",
    "KEYWORD_ENDS",
    "NUMBER_HEX",
    "NUMBER_BIN",
    "NUMBER_DEC",
    "IDENTIFIER"
]

class TokenType:
    SYM = 0
    REG8 = 1
    REG16 = 2
    REG32 = 3
    SEGREG = 4
    DIRECTIVE = 5
    INSTRUCTION = 6

    KEYWORD_SEGMENT = 7
    KEYWORD_END = 8
    KEYWORD_ENDS = 9,

    NUMBER_HEX = 10,
    NUMBER_BIN = 11,
    NUMBER_DEC = 12,
    IDENTIFIER = 13,
    
    @staticmethod
    def toStringValue(tokenType):
        if tokenType < 0 || tokenType > len(tokenMap):
            return "unknown"
        return tokenMap[tokenType]
