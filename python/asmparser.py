from token import Token

SPLIT_CHARS = [ '\t', ' ', '\n', '[', ']', '.', ',', '_','+', '-', ':' ]

def proceed_tokens(tokens, file):
    real_tokens = []
    for token in tokens:
        error, t = Token.create(token[0], file, token[1], token[2])
        if error != None: return (error, None)
        real_tokens.append(t)
    return (None, real_tokens)

def get_tokens(file, source):
    token = ""
    tokens = []
    real_tokens = []

    line = 0
    char = 0
    lastContains = False

    for c in source:
     
        if c in SPLIT_CHARS:
            tokens += (token, line, char)
            token = c
            lastContains = True
        else:
            if (lastContains):
                tokens += (token, line, char)
                token = ""

            lastContains = False
            token += c

        if (c == '\n'):
            error, t = proceed_tokens(tokens, file)
            if(error != None): return (error, None)
            real_tokens += t

            line += 1
            char = 0
        else:
            char += 1

    tokens += (token, line, char)
    error, t = proceed_tokens(tokens, file)
    if(error != None): return (error, None)
    real_tokens += t

    return (None, real_tokens)