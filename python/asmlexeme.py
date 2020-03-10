from asmusersegment import ASMUserSegment
from ttype import TokenType
from error import Error

INSTRUCTION_TYPES = [
    TokenType.INSTRUCTION,
    TokenType.DIRECTIVE,
    TokenType.KEYWORD_ENDS,
    TokenType.KEYWORD_END,
    TokenType.KEYWORD_SEGMENT
]


class ASMLexemeStructure:
    def __init__(self):
        self.has_name = 0
        self.has_instruction = 0
        self.has_operands = 0
        self.instruction_index = 0
        self.operand_indices = []
        self.operand_lengths = []

    def __str__(self, i=0):
        res = ""
        if self.has_name:
            res += "{:3} |".format(i)
        else:
            res += "{:3} |".format("--")

        if self.has_instruction:
            res += "{:3} {:3} |".format(i + self.instruction_index, 1)

        if self.has_operands and self.has_instruction:
            for j, index in enumerate(self.operand_indices):
                res += "{:3} {:3}{}".format(i + index, self.operand_lengths[j],
                                            " |" if j != len(self.operand_lengths) else "")
        return res


class ASMLexeme:
    def __init__(self, program):
        self.program = program
        self.tokens = []
        self.structure = None

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
        # **something** SEGMENT
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

        # **something** ENDS
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

        # **something**:
        if len(self.tokens) >= 2 and \
                self.tokens[0].type == TokenType.IDENTIFIER and \
                self.tokens[1].type == TokenType.SYM and self.tokens[1].string_value == ":":

            label = next((x for x in self.program.labels if x == self.tokens[0].string_value), None)
            if label is not None:
                return Error("SameLabelAreadyExists", self.tokens[0])

            self.tokens[0].type = TokenType.LABEL
            self.program.labels.append(self.tokens[0])

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
        structure.operand_indices.append(offset)
        structure.operand_lengths.append(0)
        for token in self.tokens[offset:]:
            if token.type == TokenType.SYM and token.string_value == ",":
                structure.operand_indices.append(structure.operand_lengths[operand] + operand + 1 + offset)
                structure.operand_lengths.append(0)
                operand += 1
            else:
                structure.operand_lengths[operand] += 1

        return structure
