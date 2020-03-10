from asmtoken import ASMToken

SPLIT_CHARS = ['\t', ' ', '\n', '[', ']', '.', ',', '_', '+', '-', ':']

class ASMParser:
    @staticmethod
    def save_tokens(tokens, file):
        real_tokens = []
        for token in [t for t in tokens if t[0].strip() != ""]:
            error, t = ASMToken.create(token[0], file, token[1], token[2])
            if error != None: return (error, None)
            real_tokens.append(t)
        return (None, real_tokens)

    @staticmethod
    def append_user_data_types(tokens):
        pass

    @staticmethod
    def get_lexemes(file, source):
        token = ""
        tokens = []
        real_tokens = []

        line = 0
        char = 0
        last_contains = False

        for c in source:

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
                error, t = ASMParser.save_tokens(tokens, file)
                if error != None: return (error, None)

                if len(t) != 0:
                    real_tokens.append(t)

                tokens = []

                line += 1
                char = 0
            else:
                char += 1

        tokens.append((token, line, char))
        error, t = ASMParser.save_tokens(tokens, file)
        if error != None: return (error, None)

        if len(t) != 0:
            real_tokens.append(t)

        return (None, real_tokens)
