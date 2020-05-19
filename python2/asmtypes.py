EXP_PREFIX = 0x0F
SEGMENT_PREFIX = \
    {
        "es": 0x26,
        "cs": 0x2E,
        "ss": 0x36,
        "ds": 0x3E,
        "fs": 0x64,
        "gs": 0x65,
    }

REGISTER_CODES = \
    {
        "eax": 0b000,
        "ecx": 0b001,
        "edx": 0b010,
        "ebx": 0b011,
        "esp": 0b100,
        "ebp": 0b101,
        "esi": 0b110,
        "edi": 0b111,
        "al": 0b000,
        "cl": 0b001,
        "dl": 0b010,
        "bl": 0b011,
        "ah": 0b100,
        "ch": 0b101,
        "dh": 0b110,
        "bh": 0b111,
    }


def to_hex(s, l=6):
    return "{:X}".format(s).rjust(l, '0')


def int_to_bytes(value):
    return [
        (value >> 24) & 0xFF,
        (value >> 16) & 0xFF,
        (value >> 8) & 0xFF,
        value & 0xFF,
    ]


def get_modrm(mod, reg, rm):
    return (mod & 0b11) << 6 | (reg & 0b111) << 3 | (rm & 0b111)


def short_to_bytes(value):
    return [(value >> 8) & 0xFF, value & 0xFF]


def byte_to_bytes(value):
    return [value & 0xFF]


class Bytes:
    def __init__(self):
        self.prefixes = []
        self.opcode = None
        self.modrm = None
        self.disp = []
        self.imm = []

    def set_exp_prefix(self):
        self.prefixes += EXP_PREFIX

    def set_segment_prefix(self, segment):
        self.prefixes += SEGMENT_PREFIX[segment]

    def set_opcode(self, opcode):
        self.opcode = opcode

    def set_mod_reg(self, reg):
        if self.modrm is None: self.modrm = 0
        self.modrm |= get_modrm(0, REGISTER_CODES[reg], 0)

    def set_mod_const(self, const):
        if self.modrm is None: self.modrm = 0
        self.modrm |= get_modrm(0, const, 0)

    def set_rm_reg(self, reg):
        if self.modrm is None: self.modrm = 0
        self.modrm |= get_modrm(0b11, 0, REGISTER_CODES[reg])

    def set_rm_mem_index(self, offset, reg):
        if self.modrm is None: self.modrm = 0
        self.modrm |= get_modrm(0b10, 0, REGISTER_CODES[reg])
        self.disp = int_to_bytes(offset)

    def set_rm_mem_direct(self, offset):
        if self.modrm is None: self.modrm = 0
        self.modrm |= get_modrm(0b00, 0, 0b101)
        self.disp = int_to_bytes(offset)

    def set_imm8(self, imm):
        self.imm = byte_to_bytes(imm)

    def set_imm32(self, imm):
        self.imm = int_to_bytes(imm)

    def set_imm_long(self, imm):
        self.imm = imm

    def __str__(self):
        result = ""
        for prefix in self.prefixes:
            result += "{}| ".format(to_hex(prefix, 2))
        if self.opcode is not None:
            result += "{} ".format(to_hex(self.opcode, 2))
        if self.modrm is not None:
            result += "{} ".format(to_hex(self.modrm, 2))

        if len(self.disp) != 0:
            for byte in self.disp:
                result += "{}".format(to_hex(byte, 2))
            result += " "

        if len(self.imm) != 0:
            for byte in self.imm:
                result += "{}".format(to_hex(byte, 2))
            result += " "

        return result


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


class InstructionPrototype():
    def __init__(self, name, opcode, op_count=0, op1=0, op2=0, modrm=False, mc=0, imm=0, packed=False):
        self.name = name
        self.opcode = opcode
        self.op_count = op_count
        self.op1 = op1
        self.op2 = op2
        self.modrm = modrm
        self.mc = mc
        self.imm = imm
        self.packed = packed


class InstructionInfo:
    LabelForward = 0
    LabelBackward = 1
    Memory8 = 2
    Memory32 = 3
    Register8 = 4
    Register32 = 5
    IMM8 = 6
    IMM32 = 7

    def __init__(self):
        self.op1_type = 0
        self.op2_type = 0
        self.segment_change = None
        self.imm = 0

        self.direct_memory = False
        self.memory_index = None
        self.memory_base = None
        self.instruction_prot = None


class UserName:
    def __init__(self, name, is_label, type, line):
        self.name = name
        self.is_label = is_label
        self.type = type
        self.line = line


class Storage:
    NoSegment = 0
    DataSegment = 1
    CodeSegment = 2

    def __init__(self, errout_file):
        self.errors = []
        self.errout = errout_file
        self.code_start = -1
        self.code_size = 0
        self.data_size = 0
        self.data_start = -1
        self.model_line = -1
        self.offsets = {}
        self.user_names = []
        self.bytes = {}

    def get_user_name(self, is_label, name):
        for un in self.user_names:
            if un.name == name and un.is_label == is_label:
                return un
        return None

    def has_error(self, line):
        return line in self.errors

    def set_error(self, line, error):
        self.errors.append(line)
        print("[Error]: Error at line {}: {}".format(line + 1, error), file=self.errout)

    def get_segment(self, line):
        if self.code_start == -1 and self.data_start == -1:
            return self.NoSegment
        elif self.code_start == -1:
            if line > self.data_start:
                return self.DataSegment
            else:
                return self.NoSegment
        else:
            if line < self.data_start:
                return self.NoSegment
            elif line < self.code_start:
                return self.DataSegment
            else:
                return self.CodeSegment


class Token:
    def __init__(self, type, value, line):
        self.type = type
        self.value = value
        self.line = line


class LexemeType:
    model = 0
    data_seg = 1
    code_seg = 2
    var_def = 3
    label = 4
    blank = 5
    instruction = 6
    end = 7
    unknown = 8


class TokenType:
    directive = 1
    register32 = 2
    register8 = 3
    register_seg = 4
    model = 5
    data_type = 6
    instruction = 7
    splitter = 8
    user_type = 9
    dec_number = 10
    bin_number = 11
    string = 12
    unknown = 13
