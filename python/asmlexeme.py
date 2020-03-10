from asmusersegment import ASMUserSegment
from ttype import TokenType
from error import Error


class ASMLexeme:
    def __init__(self, program):
        self.program = program

    def set_tokens(self, tokens):
        self.tokens = tokens

        error = self.append_user_type_and_labels()
        if error is not None:
            return error

        error = self.append_inline_user_type_and_labels()
        if error is not None:
            return error

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

            label = next((x for x in self.program.labels if x == token.string_value), None)
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
        if len(self.tokens) == 2 and \
                self.tokens[0].type == TokenType.IDENTIFIER and \
                self.tokens[1].type == TokenType.SYM and self.tokens[1].string_value == ":":

            label = next((x for x in self.program.labels if x == self.tokens[0].string_value), None)
            if label is not None:
                return Error("SameLabelAreadyExists", self.tokens[0])

            self.tokens[0].type = TokenType.LABEL

        return None
