from typing import Optional, List

from asmlexeme import ASMOperandType as ot
from asmlexeme import ASMLexeme

#       Cld
# CLD               FC
#
#       Dec reg
# DEC r/m8          FE      /1
# DEC r32           48      +rd
#
#       Inc mem
# [NOT USED]  INC r/m8          FE      /0
# INC r/m32         FF      /0
#
#       Or reg,reg
# OR r/m8, r8       08      /r
# OR r/m32, r32     09      /r
#
#       Cmp reg,mem
# CMP r8, r/m8      3A      /r
# CMP r32, r/m32    3B      /r
#
#       Shl mem,reg
# [NOT USED] SHL r/m8, CL      D2      /4
# SHL r/m32, CL     D3      /4
#
#       In reg,imm
# IN AL, imm8       E4      ib
# IN EAX, imm8      E5      ib
#
#       Add mem,imm
# [NOT USED] ADD r/m8, imm8    80      /0 ib
# ADD r/m32, imm32  81      /0 id
# ADD r/m32, imm8   83      /0 ib
#
#       jnz label
# JNZ rel8          75      cb
# JNZ rel32         OF 85   cw
#
#       jmp label
# JMP rel8          EB      cb
# JMP rel32         E9      cw
from error import Error

instructions = []


class ASMInstruction:
    def __init__(self, name: str, opcode: List[int], operand_types: List[ot],
                 modrm_index: int = -1,
                 constant_modrm: int = -1,
                 constant_imm: int = -1,
                 packed_register: bool = False,
                 packed_size: bool = False,
                 operand_restrictions: List[str] = None):
        self.name: str = name
        self.opcode: List[int] = opcode
        self.operand_types: List[ot] = operand_types
        self.modrm_index: int = modrm_index
        self.constant_modrm: int = constant_modrm
        self.constant_imm: int = constant_imm
        self.packed_register: bool = packed_register
        self.packed_size: bool = packed_size
        self.operand_restrictions: List[Optional[str]] = operand_restrictions

    @staticmethod
    def find_info(lexeme: 'ASMLexeme') -> Optional['ASMInstruction']:
        for instruction in instructions:

            name = lexeme.structure.get_instruction(lexeme)

            if name.string_value != instruction.name:
                continue

            if len(lexeme.structure.operands) != len(instruction.operand_types):
                continue

            wrong = False
            for index, operand in enumerate(instruction.operand_types):
                if lexeme.structure.operands[index].type != operand:
                    wrong = True
                    break

            if wrong:
                continue

            return instruction

        return None

    def check_op_restrictions(self, lexeme: 'ASMLexeme') -> Optional[Error]:
        if self.operand_restrictions is None:
            return None

        for index, restriction in enumerate(self.operand_restrictions):
            if restriction is not None:
                token = lexeme.structure.operands[index].token
                if token.string_value != restriction:
                    return Error("ExpectedAnotherOperand", token)

        return None

    @staticmethod
    def get_size(lexeme: 'ASMLexeme') -> int:
        size = 0
        if lexeme.instruction is not None:
            # Instruction
            inst: 'ASMInstruction' = lexeme.instruction

            # Op code
            size += len(inst.opcode)

            if inst.modrm_index != -1:
                size += 1  # ModRegR/M byte

            if inst.constant_imm != -1:
                size += inst.constant_imm  # Constants or Labels offset

            # If operation has mem destination/src
            if ot.Mem in inst.operand_types:
                # in out case we always have SIB byte and 4 byte displacement
                # HARDCOODE =3
                size += 1  # SIB
                size += 4  # Displacement

            for operand in lexeme.structure.operands:
                if operand.segment_prefix is not None:
                    size += 1  # Replace segment prefix

        else:
            # Variable declaration
            directive = lexeme.structure.get_instruction(lexeme)
            if directive.string_value == "db":
                size += 1
            elif directive.string_value == "dw":
                size += 2
            else:
                size += 4  # DD

        return size


instructions = [
    ASMInstruction("cld", [0xFC], []),

    ASMInstruction("dec", [0xFE], [ot.Register8], modrm_index=0, constant_modrm=1),
    ASMInstruction("dec", [0x48], [ot.Register32], packed_register=True),

    ASMInstruction("inc", [0xFF], [ot.Mem], modrm_index=0, constant_modrm=0),

    ASMInstruction("or", [0x08], [ot.Register8, ot.Register8], modrm_index=0),
    ASMInstruction("or", [0x89], [ot.Register32, ot.Register32], modrm_index=0),

    ASMInstruction("cmp", [0x3A], [ot.Register8, ot.Mem], modrm_index=1),
    ASMInstruction("cmp", [0x3B], [ot.Register32, ot.Mem], modrm_index=1),

    ASMInstruction("shl", [0xD3], [ot.Mem, ot.Register8], modrm_index=0, constant_modrm=4,
                   operand_restrictions=[None, "cl"]),

    ASMInstruction("in", [0xE4], [ot.Register8, ot.Constant8], operand_restrictions=["al", None], constant_imm=1),
    ASMInstruction("in", [0xE5], [ot.Register32, ot.Constant8], operand_restrictions=["eax", None], constant_imm=1),

    ASMInstruction("add", [0x81], [ot.Mem, ot.Constant8], modrm_index=0, constant_modrm=0, constant_imm=1),
    ASMInstruction("add", [0x83], [ot.Mem, ot.Constant32], modrm_index=0, constant_modrm=0, constant_imm=4),

    ASMInstruction("jnz", [0x75], [ot.LabelBackward], constant_imm=1),
    ASMInstruction("jnz", [0x0F, 0x85], [ot.LabelForward], constant_imm=4),

    ASMInstruction("jmp", [0xEB], [ot.LabelBackward], constant_imm=1),
    ASMInstruction("jmp", [0xE9], [ot.LabelForward], constant_imm=4),
]
