from asmprogram import ASMProgram

TEST_FILE = "test.asm"


def print_lexeme(lexeme):
    for i, token in enumerate(lexeme.tokens):
        print("{}. {}".format(str(i).rjust(2, "0"), token.to_ded_style()))


def print_et2_table(program):
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


def print_et3_tables(program):
    print("Segments table")
    print("+==========================================+")
    print("| # | Segment Name  | Bit depth |  Offset  |")
    print("+==========================================+")
    for i, segment in enumerate(program.user_segments):
        print("|{:2} |     {:5}     |    32     | {:5}    |"
              .format(i, segment.open.string_value, segment.open.line))
    print("+==========================================+")
    print()

    print("Segment register destinations")
    print("+======================================+")
    print("| # | Segment Register  | Destination  |")
    print("+======================================+")
    print("| 0 |       DS          |    {:5}     |".format(program.user_segments[0].open.string_value))
    print("| 1 |       CS          |    {:5}     |".format(program.user_segments[1].open.string_value))
    print("| 2 |       SS          |   NOTHING    |")
    print("| 3 |       ES          |   NOTHING    |")
    print("| 4 |       GS          |   NOTHING    |")
    print("| 5 |       FS          |   NOTHING    |")
    print("+======================================+")
    print()

    print("User defined names")
    print("+==============================================+")
    print("| # |    Name    |   Type   |   Seg   | Offset |")
    print("+==============================================+")
    print("+==============================================+")

if __name__ == "__main__":
    with open(TEST_FILE, "r") as file:
        source = file.read().lower()

    program = ASMProgram(source, TEST_FILE)

    error = program.parse()
    if error is not None:
        print(error)
        exit(1)

    error = program.first_pass()
    if error is not None:
        print(error)
        exit(1)

    #print_et2_table(program)
    print_et3_tables(program)