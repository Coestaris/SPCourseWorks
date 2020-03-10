from asmtoken import ASMToken
from asmlexeme import ASMLexeme

SPLIT_CHARS = ['\t', ' ', '\n', '[', ']', '.', ',', '_', '+', '-', ':']

class ASMParser:
    @staticmethod
    def save_tokens(tokens, program):
        real_tokens = []
        lexeme = ASMLexeme(program)
        for token in [t for t in tokens if t[0].strip() != ""]:

            error, t = ASMToken.create(token[0], lexeme, token[1], token[2])
            if error != None: return (error, None)

            real_tokens.append(t)

        error = lexeme.set_tokens(real_tokens)
        if error != None: return (error, None)

        return (None, lexeme)

    @staticmethod
    def append_user_data_types(tokens):
        pass

    @staticmethod
    def get_lexemes(program):
        token = ""
        tokens = []
        real_tokens = []

        line = 0
        char = 0
        last_contains = False

        for c in program.source:

            if c in SPLIT_CHARS:
                tokens.append((token, line, char))
                token = c
                last_contains = True
            else:
                if last_contains:
                    tokens.append((token, line, char))
                    token = ""

                last_contains = False
                token += c

            if c == '\n':
                error, t = ASMParser.save_tokens(tokens, program)
                if error != None: return (error, None)

                if len(t.tokens) != 0:
                    real_tokens.append(t)

                tokens = []

                line += 1
                char = 0
            else:
                char += 1

        tokens.append((token, line, char))
        error, t = ASMParser.save_tokens(tokens, program)
        if error != None: return (error, None)

        if len(t.tokens) != 0:
            real_tokens.append(t)

        return (None, real_tokens)


class ASMProgram:
    def __init__(self, source, file_name):
        self.source = source
        self.file_name = file_name
        self.lexemes = []
        self.user_segments = []
        self.labels = []

    def parse(self):
        error, self.lexemes = ASMParser.get_lexemes(self)
        if error != None: return error

        pass

