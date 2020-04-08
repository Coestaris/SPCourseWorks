from asmbytes import to_hex
from asmlexeme import ASMLexeme
from asmprogram import ASMProgram
from ttype import TokenType
import sys

TEST_FILE = "test.asm"
LOG_FILE = "log.dat"


class Logger(object):
    def __init__(self) -> None:
        self.terminal = sys.stdout
        self.log = open(LOG_FILE, "w")

    def write(self, message: str) -> None:
        self.terminal.write(message)
        self.log.write(message)

    def flush(self) -> None:
        self.terminal.flush()
        self.log.flush()


def print_lexeme(lexeme: ASMLexeme) -> None:
    for i, token in enumerate(lexeme.tokens):
        print("{}. {}".format(str(i).rjust(2, "0"), token.to_ded_style()))


def print_et2_table(program: ASMProgram) -> None:
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


def print_et3_tables(program: ASMProgram, print_source: bool) -> None:
    if print_source:
        print("+=============================================+")
        print("| #  || Offset ||        Source")
        print("+=============================================+")
        for i, lexeme in enumerate(program.lexemes):
            hex_offset = to_hex(abs(lexeme.offset))

            if lexeme.error is None:
                instruction = lexeme.structure.get_instruction(lexeme)
                if instruction.type == TokenType.KEYWORD_END:
                    hex_offset = "----"
            else:
                hex_offset = " ERR"

            print("| {} ||  {:4}  ||  {}".format(str(i).rjust(2, "0"), hex_offset, lexeme.to_pretty_source(True)))

        print("+=============================================+")
        print()

    print("Segments table:")
    print("+==========================================+")
    print("| # | Segment Name  | Bit depth |   Size   |")
    print("+==========================================+")
    for i, segment in enumerate(program.user_segments):
        print("|{:2} |     {:5}     |    32     |   {:5}  |"
              .format(i,
                      segment.open.string_value,
                      to_hex(segment.size),
                      to_hex(segment.open.line)))
    print("+==========================================+")
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


def print_et4_table(program: ASMProgram):
    print("+======================================================+")

    for i, lexeme in enumerate(program.lexemes):
        hex_offset = to_hex(abs(lexeme.offset))

        if lexeme.error is None:
            instruction = lexeme.structure.get_instruction(lexeme)
            if instruction.type == TokenType.KEYWORD_END:
                hex_offset = "----"
        else:
            hex_offset = " ERR"

        print("| {} ||  {:4}  |  {:20}  :  {}".format(
            str(i).rjust(2, "0"),
            hex_offset,
            "" if lexeme.bytes is None else lexeme.bytes.to_pretty_string(),
            lexeme.to_pretty_source(True)))
    print("+======================================================+")

    print_et3_tables(program, False)


if __name__ == "__main__":

    # Comment it if youre scared of overriding STDOUT AHHAHAHHAHAH
    sys.stdout = Logger()

    with open(TEST_FILE, "r") as file:
        source = file.read().lower()

    program = ASMProgram(source, TEST_FILE)

    program.parse()
    program.first_pass()

    program.second_pass()

    #print_et2_table(program)
    # print_et3_tables(program, true)
    print_et4_table(program)

    print()
    print("\nGot {} errors".format(program.print_errors()))
