from asmprogram import ASMProgram

TEST_FILE = "test.asm"

if __name__ == "__main__":
    with open(TEST_FILE, "r") as file:
        source = file.read().lower()
    program = ASMProgram(source, TEST_FILE)

    error = program.parse()
    if error is not None:
        print(error)
        exit(1)

    print("TOKENS: ")
    for lexeme in program.lexemes:
        print(str(lexeme))

    print()
    print()
    print("INDEXED TOKENS: ")
    print(program.to_indexed_table())

    print()
    print("SENTENCE TABLE: ")
    print(program.to_sentence_table())
