from typing import Optional, List

from asmbytes import InstructionBytes
from asmstructures import ASMUserSegment, ASMVariable
from asmtoken import ASMToken
from ttype import TokenType
from error import Error

INSTRUCTION_TYPES = [
    TokenType.INSTRUCTION,
    TokenType.DIRECTIVE,
    TokenType.KEYWORD_ENDS,
    TokenType.KEYWORD_END,
    TokenType.KEYWORD_SEGMENT
]


class ASMOperandType:
    Register8 = 1
    Register32 = 2
    Constant8 = 3
    Constant32 = 4
    LabelForward = 5
    LabelBackward = 6
    Mem8 = 7
    Mem32 = 8


class ASMOperandInfo:
    def __init__(self):
        self.type: int = 0
        self.length: int = 0
        self.index: int = 0
        self.operand_tokens: List[ASMToken] = []
        self.token: ASMToken = None
        self.segment_prefix: Optional[ASMToken] = None
        self.sum_tk1: Optional[ASMToken] = None
        self.sum_tk2: Optional[ASMToken] = None


class ASMLexemeStructure:
    def __init__(self):
        self.has_name: bool = False
        self.has_instruction: bool = False
        self.has_operands: bool = False
        self.instruction_index: int = True
        self.operands: List[ASMOperandInfo] = []

    def get_operands_info(self, parent: 'ASMLexeme') -> None:

        if not self.has_instruction or not self.has_operands:
            return None

        for operand in self.operands:
            operand.operand_tokens = parent.tokens[operand.index: operand.index + operand.length]

            if len(operand.operand_tokens) == 1:
                # reg or constant
                token = operand.operand_tokens[0]
                operand.token = token

                if token.type == TokenType.REG8:
                    operand.type = ASMOperandType.Register8

                elif token.type == TokenType.REG16 or token.type == TokenType.REG32:
                    operand.type = ASMOperandType.Register32

                elif token.type == TokenType.NUMBER_DEC or \
                        token.type == TokenType.NUMBER_BIN or \
                        token.type == TokenType.NUMBER_HEX:
                    value = token.get_num_value()
                    if value <= 255:
                        operand.type = ASMOperandType.Constant8
                    else:
                        operand.type = ASMOperandType.Constant32

                elif token.type == TokenType.LABEL:

                    # here we must determine labeling type: forward or backward
                    # it will be useful for future
                    label = [x for x in parent.program.labels if x.string_value == token.string_value][0]
                    if label.line > token.line:
                        # declared after current line
                        operand.type = ASMOperandType.LabelForward
                    else:
                        operand.type = ASMOperandType.LabelBackward

                else:
                    parent.error = Error("WrongTokenInOperand", operand.token)
                    return
            else:
                # mem
                # VAR [ AAA + BBB ]    = 6 tokens
                if len(operand.operand_tokens) < 6:
                    parent.error = Error("WrongInstructionFormat", parent.tokens[0])
                    return

                offset = 0
                if operand.operand_tokens[0].type == TokenType.SEGREG:
                    offset += 2
                    operand.segment_prefix = operand.operand_tokens[0]

                    # ES : VAR [ AAA + BBB ]    = 8 tokens
                    if len(operand.operand_tokens) != 8:
                        parent.error = Error("WrongInstructionFormat", operand.token)
                        return

                if (operand.operand_tokens[offset + 1].string_value != "[" or
                        operand.operand_tokens[offset + 3].string_value != "+" or
                        operand.operand_tokens[offset + 5].string_value != "]"):
                    parent.error = Error("WrongInstructionFormat", operand.operand_tokens[offset])
                    return

                operand.token = operand.operand_tokens[offset]
                operand.sum_tk1 = operand.operand_tokens[offset + 2]
                operand.sum_tk2 = operand.operand_tokens[offset + 4]

                variable = next((x for x in parent.program.variables if x.name.string_value == operand.token.string_value),
                                None)

                if variable is None:
                    parent.error = Error("UnknownVariable", operand.token)
                    return

                if variable.directive.string_value == "db":
                    operand.type = ASMOperandType.Mem8
                elif variable.directive.string_value == "dd":
                    operand.type = ASMOperandType.Mem32
                else:
                    parent.error = Error("UnexpectedVariableType", operand.token)
                    return

        pass

    def get_instruction(self, parent: 'ASMLexeme') -> ASMToken:
        return parent.tokens[self.instruction_index]

    def get_name(self, parent: 'ASMLexeme') -> ASMToken:
        return parent.tokens[0]

    def __str__(self, i: int = 0):
        res = ""
        if self.has_name:
            res += "{:3} |".format(i)
        else:
            res += "{:3} |".format("--")

        if self.has_instruction:
            res += "{:3} {:3} |".format(i + self.instruction_index, 1)

        if self.has_operands and self.has_instruction:
            for j, operand in enumerate(self.operands):
                res += "{:3} {:3}{}".format(i + operand.index, operand.length,
                                            " |" if j != len(self.operands) else "")
        return res


class ASMLexeme:
    def __init__(self, program: 'ASMProgram'):
        self.program: 'ASMProgram' = program
        self.tokens: List[ASMToken] = []
        self.error: Optional[Error] = None
        self.structure: ASMLexemeStructure = None
        self.offset: int = -1
        self.size: int = 0
        self.bytes: Optional[InstructionBytes] = None
        self.instruction: Optional['ASMInstruction'] = None
        self.segment: Optional[ASMUserSegment] = None

    def set_tokens(self, tokens: List[ASMToken]) -> Optional[Error]:
        self.tokens = tokens

        error = self.append_user_type_and_labels()
        if error is not None:
            return error

        self.structure = self.to_structure()
        return None

    def __str__(self):
        str = ""
        i = 0
        for token in self.tokens:
            str += "{:30}".format(token.to_ded_style())
            if i != 0 and i % 4 == 0:
                str += '\n '
            i += 1

        return "[{}]".format(str.strip())

    def append_inline_user_type_and_labels(self) -> Optional[Error]:
        for token in self.tokens:
            user_segment = next((x for x in self.program.user_segments if x.name() == token.string_value), None)
            if user_segment is not None:
                token.type = TokenType.USER_SEGMENT
                continue

            label = next((x for x in self.program.labels if x.string_value == token.string_value), None)
            if label is not None:
                token.type = TokenType.LABEL
                continue

        return None

    def append_user_type_and_labels(self) -> Optional[Error]:
        # Segment declaration
        if len(self.tokens) == 2 and \
                self.tokens[0].type == TokenType.IDENTIFIER and \
                self.tokens[1].type == TokenType.KEYWORD_SEGMENT:

            self.tokens[0].type = TokenType.USER_SEGMENT
            user_segment = next((x for x in self.program.user_segments if x.name() == self.tokens[0].string_value),
                                None)
            if user_segment is not None:
                return Error("SameUserSegmentAlreadyExists", self.tokens[0])

            user_segment = ASMUserSegment()
            user_segment.open = self.tokens[0]
            self.program.user_segments.append(user_segment)

        # Segment ending
        if len(self.tokens) == 2 and \
                self.tokens[0].type == TokenType.IDENTIFIER and \
                self.tokens[1].type == TokenType.KEYWORD_ENDS:

            user_segment = next((x for x in self.program.user_segments if x.name() == self.tokens[0].string_value),
                                None)
            if user_segment is None:
                return Error("UserSegmentNamesMismatch", self.tokens[0])

            if user_segment.closed():
                return Error("SpecifiedUserSegmentAlreadyClosed", self.tokens[0])

            self.tokens[0].type = TokenType.USER_SEGMENT
            user_segment.close = self.tokens[0]

        # Labels
        if len(self.tokens) >= 2 and \
                self.tokens[0].type == TokenType.IDENTIFIER and \
                self.tokens[1].type == TokenType.SYM and self.tokens[1].string_value == ":":

            label = next((x for x in self.program.labels if x.string_value == self.tokens[0].string_value), None)
            if label is not None:
                return Error("SameLabelAlreadyExists", self.tokens[0])

            self.tokens[0].type = TokenType.LABEL
            self.program.labels.append(self.tokens[0])

        # Variable
        if len(self.tokens) == 3 and \
                self.tokens[0].type == TokenType.IDENTIFIER and \
                self.tokens[1].type == TokenType.DIRECTIVE:

            var = next((x for x in self.program.variables if x.name.string_value == self.tokens[0].string_value), None)

            if var is not None:
                return Error("SameVariableAlreadyExists", self.tokens[0])

            self.program.variables.append(ASMVariable(self.tokens[1], self.tokens[0], self.tokens[2]))

        return None

    def has_label(self) -> bool:
        return len(self.tokens) >= 2 and self.tokens[0].type == TokenType.LABEL

    def to_structure(self) -> ASMLexemeStructure:
        structure = ASMLexemeStructure()
        structure.instruction_index = 0
        structure.has_instruction = True
        structure.has_operands = True
        structure.has_name = False

        offset = 0
        if self.has_label():
            structure.has_name = True
            offset += 2

        if len(self.tokens) <= offset:
            # Has only label
            structure.has_instruction = False
            return structure

        if self.tokens[0].type == TokenType.IDENTIFIER or self.tokens[0].type == TokenType.USER_SEGMENT:
            # Has name
            structure.has_name = True
            offset += 1

        structure.has_instruction = next((x for x in self.tokens if x.type in INSTRUCTION_TYPES), None) \
                                    is not None

        if not structure.has_instruction:
            # We dont need it anymore
            return structure

        structure.instruction_index = offset
        offset += 1  # Instruction offset

        if len(self.tokens) <= offset:
            # Has only instruction (or + name)
            structure.has_operands = False
            return structure

        operand = 0
        structure.operands.append(ASMOperandInfo())
        structure.operands[-1].index = offset
        structure.operands[-1].length = 0
        for token in self.tokens[offset:]:
            if token.type == TokenType.SYM and token.string_value == ",":
                structure.operands.append(ASMOperandInfo())
                structure.operands[-1].index = structure.operands[operand].length + operand + 1 + offset
                structure.operands[-1].length = 0

                operand += 1
            else:
                structure.operands[operand].length += 1

        return structure

    def to_pretty_source(self, has_indentation: bool) -> str:
        str = ""
        for token in self.tokens:
            has_space_before = False
            has_space_after = False
            value = token.string_value

            if token.type == TokenType.SYM and token.string_value == ",":
                has_space_after = True

            elif token.type == TokenType.SYM and token.string_value == "+":
                has_space_after = True
                has_space_before = True

            elif token.type == TokenType.INSTRUCTION or token.type == TokenType.DIRECTIVE or \
                    token.type == TokenType.USER_SEGMENT:
                has_space_after = True

            if token.type == TokenType.DIRECTIVE and (
                    token.string_value == "db" or token.string_value == "dd" or token.string_value == "dw"):
                has_space_before = True

            if token.type == TokenType.DIRECTIVE or token.type == TokenType.KEYWORD_END \
                    or token.type == TokenType.KEYWORD_ENDS \
                    or token.type == TokenType.KEYWORD_SEGMENT:
                value = value.upper()

            str += (" " if has_space_before else "") + \
                   value + \
                   (" " if has_space_after else "")

        if self.error:
            return "   " + str

        identation = ""
        if has_indentation:
            directive = self.structure.get_instruction(self)
            if (directive.string_value == "db" or directive.string_value == "dd" or directive.string_value == "dw"):
                identation = "   "
            else:
                if self.segment is not None:
                    identation = "   "
                if not self.structure.has_name and directive.type != TokenType.KEYWORD_END:
                    identation += "   "

        return identation + str
