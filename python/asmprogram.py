from asmtoken import ASMToken
from asmlexeme import ASMLexeme
from error import Error
from ttype import TokenType

SPLIT_CHARS = ['\t', ' ', '\n', '[', ']', '.', ',', '_', '+', '-', ':']


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


class ASMProgram:
    def __init__(self, source, file_name):
        self.source = source
        self.file_name = file_name
        self.lexemes = []
        self.user_segments = []
        self.labels = []

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

    def to_sentence_table(self):
        res = ""
        i = 0
        for index, lexeme in enumerate(self.lexemes):
            # res += str(lexeme) + "\n"

            label_index, inst_index, op_indices, op_length, has_no_operands, has_no_instruction \
                = lexeme.to_sentence_table()

            res += "{:-2} |".format(index)

            if label_index != -1:
                res += "{:4} |".format(label_index + i)
            else:
                res += "{:4} |".format("none")
                label_index = 0

            if not has_no_instruction:
                res += "{:4} {:4} |".format(i + inst_index, 1)

            if not has_no_instruction and not has_no_operands:
                for j in range(0, len(op_indices)):
                    res += "{:4} {:4}{}".format(i + label_index + op_indices[j], op_length[j],
                                                "" if j == len(op_indices) - 1 else " |")

            if index != len(self.lexemes) - 1:
                res += "\n"

            i += len(lexeme.tokens)

        return res