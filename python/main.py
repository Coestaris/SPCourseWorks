from asmparser import ASMParser

TEST_FILE = "test.asm"

if __name__ == "__main__":
    with open(TEST_FILE, "r") as file:
        source = file.read().lower()

    error, lexemes = ASMParser.get_lexemes(TEST_FILE, source)
    if error != None:
        print(error)
        exit(1)

    for lexeme in lexemes:
        for token in lexeme:
            print(token.string_value, end=' ')
        print()
