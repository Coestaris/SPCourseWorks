from asmprogram import ASMProgram

TEST_FILE = "test.asm"

def print_lexeme(lexeme):
    for i, token in enumerate(lexeme.tokens):
        print("{}. {}".format(str(i).rjust(2, "0"), token.to_ded_style()))

if __name__ == "__main__":
    with open(TEST_FILE, "r") as file:
        source = file.read().lower()

    program = ASMProgram(source, TEST_FILE)

    error = program.parse()
    if error is not None:
        print(error)
        exit(1)

    for lexeme in program.lexemes:
        print("Source   : {}".format(" ".join([x.string_value for x in lexeme.tokens])))
        print_lexeme(lexeme)
        print("         ========================================+")
        print("         |  LBL |  MNEM  |   Op1  |   Op2  | ....|")
        print("         |      |  I | L |  I | L |  I | L |     |")
        print("         ========================================|")
        print("         |  {} ".format(lexeme.structure))
        print()
        print()
