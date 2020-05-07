class LineStructure:
    def __init__(self):
        self.op_count = 0
        self.op1_index = 0
        self.op1_len = 0
        self.op2_index = 0
        self.op2_len = 0

        self.has_name = False
        self.has_instruction = False
        self.instruction_index = 0

    def get_op1(self, tokens):
        return tokens[self.op1_index: self.op1_index + self.op1_len]

    def get_op2(self, tokens):
        return tokens[self.op2_index: self.op2_index + self.op2_len]

    def get_instruction(self, tokens):
        return tokens[self.instruction_index]

    def get_ops(self):
        if self.op_count == 0:
            return []
        elif self.op_count == 1:
            return [(self.op1_index, self.op1_len)]
        else:
            return [(self.op1_index, self.op1_len), (self.op2_index, self.op2_len)]


class Storage:
    def __init__(self, errout_file):
        self.errors = []
        self.errout = errout_file

    def has_error(self, line):
        return line in self.errors

    def set_error(self, line, error):
        self.errors.append(line)
        print("[Error]: Error at line {}: {}\n".format(line + 1, error), file=self.errout)


class Token:
    def __init__(self, type, value, line):
        self.type = type
        self.value = value
        self.line = line
