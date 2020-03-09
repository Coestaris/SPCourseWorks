class TokenType:
    SYM = 1
    REG8 = 2
    REG16 = 3
    REG32 = 4
    SEGREG = 5
    DIRECTIVE = 6
    INSTRUCTION = 7

    KEYWORD_SEGMENT = 8
    KEYWORD_END = 9
    KEYWORD_ENDS = 10,

    NUMBER_HEX = 11,
    NUMBER_BIN = 12,
    NUMBER_DEC = 13,
    IDENTIFIER = 14,

    @staticmethod
    def toStringValue(tokenType):
        if tokenType == TokenType.SYM:
            return "SYM"
        elif tokenType == TokenType.REG8:
            return "REG8"
        elif tokenType == TokenType.REG16:
            return "REG16"
        elif tokenType == TokenType.REG32:
            return "REG32"
        elif tokenType == TokenType.SEGREG:
            return "SEGREG"
        elif tokenType == TokenType.DIRECTIVE:
            return "DIRECTIVE"
        elif tokenType == TokenType.INSTRUCTION:
            return "INSTRUCTION"
        elif tokenType == TokenType.KEYWORD_SEGMENT:
            return "KEYWORD_SEGMENT"
        elif tokenType == TokenType.KEYWORD_END:
            return "KEYWORD_END"
        elif tokenType == TokenType.KEYWORD_ENDS:
            return "KEYWORD_ENDS"
        elif tokenType == TokenType.NUMBER_HEX:
            return "NUMBER_HEX"
        elif tokenType == TokenType.NUMBER_BIN:
            return "NUMBER_BIN"
        elif tokenType == TokenType.NUMBER_DEC:
            return "NUMBER_DEC"
        elif tokenType == TokenType.IDENTIFIER:
            return "IDENTIFIER"
        else:
            return "unkown"