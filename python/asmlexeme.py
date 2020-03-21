from asmstructures import ASMUserSegment, ASMVariable
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
    Label = 5
    Mem = 6


class ASMOperandInfo:
    def __init__(self):
        self.type = 0
        self.length = 0
        self.index = 0
        self.operand_tokens = []
        self.token = None
        self.segment_prefix = None
        self.sum_tk1 = None
        self.sum_tk2 = None

class ASMLexemeStructure:
    def __init__(self):
        self.has_name = 0
        self.has_instruction = 0
        self.has_operands = 0
        self.instruction_index = 0
        self.operands = []

    def get_operands_info(self, parent):
        if not self.has_instruction or not self.has_operands:
            return None

        for operand in self.operands:
            operand.operand_tokens = parent.tokens[operand.index : operand.index + operand.length]

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
                    if value < 255:
                        operand.type = ASMOperandType.Constant8
                    else:
                        operand.type = ASMOperandType.Constant32
                elif token.type == TokenType.LABEL:
                    operand.type = ASMOperandType.Label
                else:
                    return Error("WrongTokenInOperand", operand.token)
            else:
                # mem
                # VAR [ AAA + BBB ]    = 6 tokens
                if len(operand.operand_tokens) < 6:
                    return Error("WrongInstructionFormat", operand.token)

                offset = 0
                operand.type = ASMOperandType.Mem
                if operand.operand_tokens[0].type == TokenType.SEGREG:
                    offset += 2
                    operand.segment_prefix = operand.operand_tokens[0]

                    # ES : VAR [ AAA + BBB ]    = 8 tokens
                    if len(operand.operand_tokens) != 8:
                        return Error("WrongInstructionFormat", operand.token)

                operand.token = operand.operand_tokens[offset]
                operand.sum_tk1 = operand.operand_tokens[offset + 2]
                operand.sum_tk2 = operand.operand_tokens[offset + 4]

        return None

    def get_instruction(self, parent):
        return parent.tokens[self.instruction_index]

    def get_name(self, parent):
        return parent.tokens[0]

    def __str__(self, i=0):
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
    def __init__(self, program):
        self.program = program
        self.tokens = []
        self.structure = None
        self.offset = 0
        self.segment = None

    def set_tokens(self, tokens):
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

    def append_inline_user_type_and_labels(self):
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

    def append_user_type_and_labels(self):
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

            label = next((x for x in self.program.labels if x == self.tokens[0].string_value), None)
            if label is not None:
                return Error("SameLabelAlreadyExists", self.tokens[0])

            self.tokens[0].type = TokenType.LABEL
            self.program.labels.append(self.tokens[0])

        # Variable
        if len(self.tokens) == 3 and \
                self.tokens[0].type == TokenType.IDENTIFIER and \
                self.tokens[1].type == TokenType.DIRECTIVE:

            var = next((x for x in self.program.variables if x == self.tokens[0].string_value), None)

            if var is not None:
                return Error("SameVariableAlreadyExists", self.tokens[0])

            self.program.variables.append(ASMVariable(self.tokens[1], self.tokens[0], self.tokens[2]))

        return None

    def has_label(self):
        return len(self.tokens) >= 2 and self.tokens[0].type == TokenType.LABEL

    def to_structure(self):
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
