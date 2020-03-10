from asmtoken import ASMToken

SPLIT_CHARS = ['\t', ' ', '\n', '[', ']', '.', ',', '_', '+', '-', ':']


def proceed_tokens(tokens, file):
    real_tokens = []
    for token in [t for t in tokens if t[0].strip() != ""]:
        print(token)
        error, t = ASMToken.create(token[0], file, token[1], token[2])
        if error != None: return (error, None)
        real_tokens.append(t)
    return (None, real_tokens)


def get_tokens(file, source):
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
            error, t = proceed_tokens(tokens, file)
            if error != None: return (error, None)
            real_tokens += t
            tokens = []

            line += 1
            char = 0
        else:
            char += 1

    tokens.append((token, line, char))
    error, t = proceed_tokens(tokens, file)
    if error != None: return (error, None)
    real_tokens += t

    return (None, real_tokens)
