from asmlexeme import ASMLexeme
from asmprogram import ASMProgram
from ttype import TokenType

TEST_FILE = "test.asm"


def print_lexeme(lexeme: ASMLexeme):
    for i, token in enumerate(lexeme.tokens):
        print("{}. {}".format(str(i).rjust(2, "0"), token.to_ded_style()))


def print_et2_table(program: ASMProgram):
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


def to_hex(s: int, l: int = 4) -> str:
    return "{:X}".format(s).rjust(l, '0')


def print_et3_tables(program: ASMProgram):
    print("+=============================================+")
    print("| #  || Offset ||        Source")
    print("+=============================================+")
    for i, lexeme in enumerate(program.lexemes):
        hex_offset = to_hex(lexeme.offset)
        if lexeme.offset <= -1 or lexeme.segment is None or not lexeme.structure.has_instruction:
            hex_offset = "----"

        print("| {} ||  {:4}  ||  {}".format(str(i).rjust(2, "0"), hex_offset, lexeme.to_pretty_source(True)))
    print("+=============================================+")
    print()

    print("Segments table:")
    print("+=====================================================+")
    print("| # | Segment Name  | Bit depth |   Size   |   Line   |")
    print("+=====================================================+")
    for i, segment in enumerate(program.user_segments):
        print("|{:2} |     {:5}     |    32     |   {:5}  |   {:5}  |"
              .format(i,
                      segment.open.string_value,
                      to_hex(segment.size),
                      to_hex(segment.open.line)))
    print("+=====================================================+")
    print()

    print("Segment register destinations:")
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

    print("User defined names:")
    i = 0
    print("+================================================+")
    print("| # |    Name    |   Type   |   Seg   |   Value  |")
    print("+================================================+")
    for label in program.labels:
        print("|{:2} |   {:7} |   LABEL   |  {:5}  |   {:5}  |"
              .format(i,
                      label.string_value,
                      label.lexeme.segment.open.string_value,
                      to_hex(abs(label.lexeme.offset))))
        i += 1
    for variable in program.variables:
        print("|{:2} |   {:7} |   {:5}   |  {:5}  |   {:5}  |"
              .format(i,
                      variable.name.string_value,
                      variable.directive.string_value.upper(),
                      variable.name.lexeme.segment.open.string_value,
                      to_hex(abs(variable.name.lexeme.offset))))
        i += 1


    print("+================================================+")


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

    # print_et2_table(program)
    print_et3_tables(program)
