from typing import List, Optional

from error import Error

EXP_PREFIX = 0x0F
SEG_PREFIX = {
    "es": 0x26,
    "cs": 0x2E,
    "ss": 0x36,
    "ds": 0x3E,
    "fs": 0x64,
    "gs": 0x65,
}
REG_CODES = {
    "eax": 0b000,
    "ecx": 0b001,
    "edx": 0b010,
    "ebx": 0b011,
    "esp": 0b100,
    "ebp": 0b101,
    "esi": 0b110,
    "edi": 0b111,
    "al":  0b000,
    "cl":  0b001,
    "dl":  0b010,
    "bl":  0b011,
    "ah":  0b100,
    "ch":  0b101,
    "dh":  0b110,
    "bh":  0b111,
}

MOD_REG = 0b11
MOD_MEM_PLUS_DISP8 = 0b01
MOD_MEM_PLUS_DISP32 = 0b10
RM_SIB = 0b100


def get_modregrm(mod: int, reg: int, rm: int) -> int:
    return (mod & 0b11) << 6 | (reg & 0b111) << 3 | (rm & 0b111)


def get_sib(scale: int, index: int, base: int) -> int:
    return (scale & 0b11) << 6 | (index & 0b111) << 3 | (base & 0b111)


def to_hex(s: int, l: int = 4) -> str:
    return "{:X}".format(s).rjust(l, '0')


class InstructionBytes:
    def __init__(self) -> None:
        self.prefixes: List[int] = []
        self.opcode: int = -1
        self.modrm: int = -1
        self.sib: int = -1
        self.disp: List[int] = []
        self.imm: List[int] = []

    def set_opcode(self, opcode: int) -> None:
        self.opcode = opcode

    def set_opcode_packed(self, packed_reg: 'Token') -> Optional[Error]:
        if packed_reg.string_value not in REG_CODES:
            return Error("UnknownRegister", packed_reg)

        self.opcode |= REG_CODES[packed_reg.string_value]
        return None

    def set_expansion_prefix(self) -> None:
        self.prefixes += [EXP_PREFIX]

    def set_seg_prefix(self, seg: 'Token') -> Optional[Error]:
        if seg.string_value not in SEG_PREFIX:
            return Error("UnknownSegmentPrefix", seg)

        self.prefixes += [SEG_PREFIX[seg.string_value]]
        return None

    def set_modrm_dest_reg(self, dest: 'Token') -> Optional[Error]:
        if self.modrm == -1:
            self.modrm = 0

        if dest.string_value not in REG_CODES:
            return Error("UnknownRegister", dest)

        self.modrm |= get_modregrm(MOD_REG, 0, REG_CODES[dest.string_value])
        return None

    def set_modrm_dest_mem(self, var: 'Token', op1: 'Token', op2: 'Token') -> Optional[Error]:
        if self.modrm == -1:
            self.modrm = 0

        if op1.string_value not in REG_CODES:
            return Error("UnknownRegister", op1)
        if op2.string_value not in REG_CODES:
            return Error("UnknownRegister", op2)

        variable = next((x for x in var.lexeme.program.variables if x.name.string_value == var.string_value), None)

        self.modrm |= get_modregrm(MOD_MEM_PLUS_DISP32, 0, RM_SIB)
        self.sib = get_sib(0, REG_CODES[op2.string_value], REG_CODES[op1.string_value])
        self.disp = int32tobytes(variable.name.lexeme.offset)
        return None

    def set_modrm_source_reg(self, source: 'Token') -> Optional[Error]:
        if self.modrm == -1:
            self.modrm = 0

        if source.string_value not in REG_CODES:
            return Error("UnknownRegister", source)

        self.modrm |= get_modregrm(0, REG_CODES[source.string_value], 0)
        return None

    def set_modrm_source_const(self, const: int) -> Optional[Error]:
        if self.modrm == -1:
            self.modrm = 0

        self.modrm |= get_modregrm(0, const, 0)
        return None

    def set_imm(self, imm: List[int]) -> None:
        self.imm = imm

    def to_bytes(self) -> List[int]:
        b = []
        b += self.prefixes

        if self.opcode != -1:
            b += self.opcode

        if self.modrm != -1:
            b += self.modrm

        if self.sib:
            b += self.sib

        b += self.disp
        b += self.imm
        return b

    def to_pretty_string(self) -> str:
        result = ""
        for prefix in self.prefixes:
            result += to_hex(prefix, 2) + ": "

        if self.opcode != -1:
            result += to_hex(self.opcode, 2) + " "

        if self.modrm != -1 and self.sib != -1:
            result += to_hex(self.modrm, 2) + to_hex(self.sib, 2) + " "
        elif self.modrm != -1:
            result += to_hex(self.modrm, 2) + " "

        if self.disp:
            result += "".join([to_hex(x, 2) for x in self.disp]) + " "

        if self.imm:
            result += "".join([to_hex(x, 2) for x in self.imm]) + " "

        return result

def int32tobytes(value: int) -> List[int]:
    return [
        (value >> 24) & 0xFF,
        (value >> 16) & 0xFF,
        (value >> 8) & 0xFF,
        value & 0xFF,
    ]


def int16tobytes(value: int) -> List[int]:
    return [
        (value >> 8) & 0xFF,
        value & 0xFF
    ]


def int8tobytes(value: int) -> List[int]:
    return [
        value & 0xFF
    ]
