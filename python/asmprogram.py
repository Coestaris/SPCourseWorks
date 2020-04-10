from typing import Optional, List, Tuple

from asmbytes import InstructionBytes
from asmstructures import ASMUserSegment, ASMVariable
from asmtoken import ASMToken
from asminstruction import ASMInstruction
from asmlexeme import ASMLexeme
from error import Error
from ttype import TokenType

SPLIT_CHARS = ['\t', ' ', '\n', '[', ']', '.', ',', '*', '+', ':']


class ASMParser:
    @staticmethod
    def save_tokens(tokens: List[Tuple[str, int, int]], program: 'ASMProgram') -> Optional[ASMLexeme]:
        real_tokens = []
        lexeme = ASMLexeme(program)
        for token in [t for t in tokens if t[0].strip() != ""]:

            error, t = ASMToken.create(token[0], lexeme, token[1], token[2])
            if error is not None:
                lexeme.error = error
                t = ASMToken(token[0], lexeme, 0, token[1], token[2])

            real_tokens.append(t)

        error = lexeme.set_tokens(real_tokens)
        if error is not None:
            lexeme.error = error
            return lexeme

        return lexeme

    @staticmethod
    def get_lexemes(program: 'ASMProgram') -> List[ASMLexeme]:
        token = ""
        tokens = []
        lexemes_list = []

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
                lexeme = ASMParser.save_tokens(tokens, program)
                if len(lexeme.tokens) != 0 or lexeme.error:
                    lexemes_list.append(lexeme)

                tokens = []

                line += 1
                char = 0
            else:
                char += 1

        tokens.append((token, line, char))
        lexeme = ASMParser.save_tokens(tokens, program)

        if len(lexeme.tokens) != 0:
            lexemes_list.append(lexeme)

        return lexemes_list

    @staticmethod
    def proceed_segments(program: 'ASMProgram') -> None:
        current_segment = None
        for lexeme in program.lexemes:
            if lexeme.error is None:
                if len(lexeme.tokens) == 2 and lexeme.tokens[1].type == TokenType.KEYWORD_SEGMENT:
                    # find segment by its name
                    current_segment = \
                        [x for x in program.user_segments if x.open.string_value == lexeme.tokens[0].string_value][0]

                elif len(lexeme.tokens) == 2 and lexeme.tokens[1].type == TokenType.KEYWORD_ENDS:
                    current_segment = None

                else:
                    lexeme.segment = current_segment
                    if current_segment is None and \
                            not (len(lexeme.tokens) == 1 and lexeme.tokens[0].type == TokenType.KEYWORD_END):
                        lexeme.error = Error("OnlyEndCouldBeWithoutSegment", lexeme.tokens[0])

        pass


class ASMProgram:
    def __init__(self, source: str, file_name: str):
        self.source: str = source
        self.file_name: str = file_name
        self.lexemes: List[ASMLexeme] = []
        self.user_segments: List[ASMUserSegment] = []
        self.labels: List[ASMToken] = []
        self.variables: List[ASMVariable] = []

    def parse(self) -> None:
        self.lexemes = ASMParser.get_lexemes(self)

        for lexeme in self.lexemes:
            if lexeme.error is None:
                lexeme.append_inline_user_type_and_labels()

        if len(self.lexemes[-1].tokens) != 1 and self.lexemes[-1].tokens[0].type != TokenType.KEYWORD_ENDS:
            self.lexemes[-1].error = Error("Program should end with ENDS keyword", None)

        pass

    def first_pass(self) -> Optional[Error]:

        # Assign segments and check lines that out of segments
        ASMParser.proceed_segments(self)

        # Fetch information about operands in lexemes with instructions
        for lexeme in self.lexemes:
            if lexeme.error is None:

                if not lexeme.structure.has_instruction and not lexeme.structure.has_name:
                    lexeme.error = Error("WrongLexeme", lexeme.tokens[0])
                    continue

                if lexeme.structure.has_name and not lexeme.has_label() and \
                        lexeme.structure.has_instruction and \
                        lexeme.structure.get_instruction(lexeme).type == TokenType.INSTRUCTION:
                    lexeme.error = Error("WrongTokenInLexeme", lexeme.tokens[0])
                    continue

                if len(lexeme.tokens) >= 2 and lexeme.tokens[0].string_value == "assume":
                    lexeme.error = Error("WrongTokenInLexeme", lexeme.tokens[0])
                    continue

                lexeme.structure.get_operands_info(lexeme)

        # Find matching instruction for our lexemes:
        offset = 0
        for lexeme in self.lexemes:
            if lexeme.error is None:

                # We cant caluclate size of directive without instruction
                if not lexeme.structure.has_instruction:
                    lexeme.offset = -offset
                    continue

                # We cant calculate size of SEGMNET, ENDS and END but it resets offset counter
                directive = lexeme.structure.get_instruction(lexeme)
                if directive.type == TokenType.KEYWORD_SEGMENT or \
                        directive.type == TokenType.KEYWORD_ENDS or \
                        directive.type == TokenType.KEYWORD_END:
                    # We assign negative values for lexemes that dont actually
                    # have offset but we need it to know their position in file for creating
                    # those useless tables
                    lexeme.offset = -offset

                    offset = 0
                    continue

                if directive.type == TokenType.INSTRUCTION:
                    # Try to find mathing instruction
                    inst = ASMInstruction.find_info(lexeme)
                    if inst is None:
                        lexeme.error = Error("UnableToFindMatchingFunction", lexeme.tokens[0])
                        continue

                    lexeme.instruction = inst

                    error = inst.check_op_restrictions(lexeme)
                    if error is not None:
                        lexeme.error = error
                        continue

                lexeme.offset = offset
                lexeme.size = ASMInstruction.get_size(lexeme)

                offset += lexeme.size
                if lexeme.segment is not None:
                    lexeme.segment.size = offset

        return None

    def second_pass(self):
        for lexeme in self.lexemes:
            if lexeme.error is None:

                #if directive.type == TokenType.KEYWORD_SEGMENT or \
                #    directive.type == TokenType.KEYWORD_ENDS or \
                #    directive.type == TokenType.KEYWORD_END:
                #    continue

                directive = lexeme.structure.get_instruction(lexeme)
                if directive.type == TokenType.INSTRUCTION or \
                    directive.type == TokenType.DIRECTIVE:
                    lexeme.bytes = InstructionBytes()
                    lexeme.error = ASMInstruction.get_bytes(lexeme)
        pass

    def print_errors(self) -> int:
        for lexeme in self.lexemes:
            if lexeme.error is not None:
                if lexeme.error.token:
                    print("Error occurred in file {}:{} at token \"{}\": {}".format(
                          lexeme.error.token.file,
                          lexeme.error.token.line + 1,
                          lexeme.error.token.string_value,
                          lexeme.error.message))
                else:
                    print("Error occurred: {}".format(lexeme.error))

        return len([a for a in self.lexemes if a.error is not None])