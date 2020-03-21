from asmtoken import ASMToken
from asmlexeme import ASMLexeme
from error import Error
from ttype import TokenType

SPLIT_CHARS = ['\t', ' ', '\n', '[', ']', '.', ',', '*', '+', '-', ':']


class ASMParser:
    @staticmethod
    def save_tokens(tokens, program):
        real_tokens = []
        lexeme = ASMLexeme(program)
        for token in [t for t in tokens if t[0].strip() != ""]:

            error, t = ASMToken.create(token[0], lexeme, token[1], token[2])
            if error is not None: return (error, None)

            real_tokens.append(t)

        error = lexeme.set_tokens(real_tokens)
        if error is not None: return (error, None)

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
                if error is not None: return (error, None)

                if len(t.tokens) != 0:
                    real_tokens.append(t)

                tokens = []

                line += 1
                char = 0
            else:
                char += 1

        tokens.append((token, line, char))
        error, t = ASMParser.save_tokens(tokens, program)
        if error is not None: return (error, None)

        if len(t.tokens) != 0:
            real_tokens.append(t)

        return (None, real_tokens)

    @staticmethod
    def proceed_segments(program):
        current_segment = None
        for lexeme in program.lexemes:
            if len(lexeme.tokens) == 2 and lexeme.tokens[1].type == TokenType.KEYWORD_SEGMENT:
                # find segment by its name
                current_segment = [x for x in program.user_segments if x.open.string_value == lexeme.tokens[0].string_value][0]

            elif len(lexeme.tokens) == 2 and lexeme.tokens[1].type == TokenType.KEYWORD_ENDS:
                current_segment = None

            else:
                lexeme.segment = current_segment
                if current_segment is None and \
                        not (len(lexeme.tokens) == 1 and lexeme.tokens[0].type == TokenType.KEYWORD_END):
                    return Error("OnlyEndCouldBeWithoutSegment", lexeme.tokens[0])

        return None


class ASMProgram:
    def __init__(self, source, file_name):
        self.source = source
        self.file_name = file_name
        self.lexemes = []
        self.user_segments = []
        self.labels = []
        self.variables = []

    def parse(self):
        error, self.lexemes = ASMParser.get_lexemes(self)
        if error is not None: return error

        for lexeme in self.lexemes:
            error = lexeme.append_inline_user_type_and_labels()
            if error is not None:
                return error

        if len(self.lexemes[-1].tokens) != 1 and self.lexemes[-1].tokens[0].type != TokenType.KEYWORD_ENDS:
            return Error("Program should end with ENDS keyword", None)

        pass

    def first_pass(self):
        # Assign segments and check lines that out of segments
        error = ASMParser.proceed_segments(self)
        if error is not None: return error

        # Fetch information about operands in lexemes with instructions
        for lexeme in self.lexemes:
            error = lexeme.structure.get_operands_info(lexeme)
            if error is not None: return error

        # Find matching instruction for our lexemes:
        

        return None