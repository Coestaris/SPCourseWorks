from typing import Optional, List

from asmlexeme import ASMOperandType as ot
from asmlexeme import ASMLexeme
import asmbytes

# cld
#               CLD                FC
# dec reg
#               DEC r8             FE      /1
#               DEC r32            48      +rd
# inc mem
#   [NOT USED]  INC r/m8           FE      /0
#               INC r/m32          FF      /0
# or reg,reg
#               OR r8, r/m8      0A      /r
#               OR r32, r/m32    0B      /r
# cmp reg,mem
#               CMP r8,    r/m8    3A      /r
#               CMP r32,   r/m32   3B      /r
# shl mem,reg
#   [NOT USED]  SHL r/m8,  CL      D2      /4
#               SHL r/m32, CL      D3      /4
# in reg,imm
#               IN AL,     imm8    E4      ib
#               IN EAX,    imm8    E5      ib
# add mem,imm
#   [NOT USED]  ADD r/m8,  imm8    80      /0 ib
#               ADD r/m32, imm32   81      /0 id
#               ADD r/m32, imm8    83      /0 ib
# jnz label
#               JNZ rel8           75      cb
#               JNZ rel32          OF 85   cw
# jmp label
#               JMP rel8           EB      cb
#               JMP rel32          E9      cw
from error import Error

instructions = []


class ASMInstruction:
    def __init__(self, name: str, opcode: List[int], operand_types: List[ot],
                 modrm_index: int = -1,
                 constant_modrm: int = -1,
                 constant_imm: int = -1,
                 packed_register: bool = False,
                 operand_restrictions: List[str] = None):
        self.name: str = name
        self.opcode: List[int] = opcode
        self.operand_types: List[ot] = operand_types
        self.modrm_index: int = modrm_index
        self.constant_modrm: int = constant_modrm
        self.constant_imm: int = constant_imm
        self.packed_register: bool = packed_register
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
            if ot.Mem8 in inst.operand_types or ot.Mem32 in inst.operand_types:
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

    @staticmethod
    def get_bytes(lexeme: 'ASMLexeme') -> Optional[Error]:

        if lexeme.instruction is not None:
            # Instruction
            inst: 'ASMInstruction' = lexeme.instruction
            bytes = lexeme.bytes

            # Opcode
            if len(inst.opcode) == 2:
                bytes.set_expansion_prefix()
                bytes.set_opcode(inst.opcode[1])
            else:
                bytes.set_opcode(inst.opcode[0])

            # Jumps are weird. Just hardcode them...
            if inst.name == "jnz" or inst.name == "jmp":
                operand = lexeme.structure.operands[0]

                lbl = next((x for x in lexeme.program.labels if x.string_value == operand.token.string_value), None)

                current_offset = abs(lexeme.offset)
                lbl_offset = abs(lbl.lexeme.offset)
                diff = lbl_offset - current_offset

                imm = []
                if inst.name == "jnz":
                    if operand.type == ot.LabelForward:
                        # I cant explain this right now....
                        imm = asmbytes.int8tobytes(diff - 2)
                        imm += [0x90] * 4
                    else:
                        imm = asmbytes.int8tobytes(diff - lexeme.size)
                else:
                    if operand.type == ot.LabelForward:
                        imm = asmbytes.int8tobytes(diff - 2)
                        imm += [0x90] * 3
                    else:
                        imm = asmbytes.int32tobytes(diff - lexeme.size)

                bytes.set_imm(imm)
                return None

            # First register packed to an opcode
            if inst.packed_register:
                error = bytes.set_opcode_packed(lexeme.structure.operands[0].token)
                if error is not None:
                    return error

            # ModRM and SIB
            if inst.modrm_index != -1:
                # dest
                operand = lexeme.structure.operands[inst.modrm_index]
                if operand.type == ot.Mem8 or operand.type == ot.Mem32:
                    error = bytes.set_modrm_dest_mem(operand.token, operand.sum_tk1, operand.sum_tk2)
                    if error is not None:
                        return error
                else:
                    # Register
                    error = bytes.set_modrm_dest_reg(operand.token)
                    if error is not None:
                        return error

                # source. Could be register of a constant field
                if inst.constant_modrm != -1:
                    error = bytes.set_modrm_source_const(inst.constant_modrm)
                    if error is not None:
                        return error

                elif len(lexeme.structure.operands) != 1:
                    # Has source
                    # ModRM points to Dest, it means that if index == 0 - source == 1
                    if inst.modrm_index == 0:
                        source = lexeme.structure.operands[1]
                    else:
                        source = lexeme.structure.operands[0]

                    error = bytes.set_modrm_source_reg(source.token)
                    if error is not None:
                        return error

            # Segment prefix
            for operand in lexeme.structure.operands:
                if operand.segment_prefix is not None:
                    error = bytes.set_seg_prefix(operand.segment_prefix)
                    if error is not None:
                        return error

            # Imm. Find constant operand and add it to IMM field
            if inst.constant_imm != -1:
                for operand in lexeme.structure.operands:
                    if operand.type == ot.Constant8:
                        bytes.set_imm(asmbytes.int8tobytes(operand.token.get_num_value()))
                        break
                    if operand.type == ot.Constant32:
                        bytes.set_imm(asmbytes.int32tobytes(operand.token.get_num_value()))
                        break

        else:
            # Variable declaration
            directive = lexeme.structure.get_instruction(lexeme)
            value = lexeme.structure.operands[0].token.get_num_value()
            bytes = []

            if directive.string_value == "db":
                bytes += asmbytes.int8tobytes(value)
            elif directive.string_value == "dw":
                bytes += asmbytes.int16tobytes(value)
            else:
                bytes += asmbytes.int32tobytes(value)  # DD

            lexeme.bytes.set_imm(bytes)

        return None


instructions = [
    ASMInstruction("cld", [0xFC], []),

    ASMInstruction("dec", [0xFE], [ot.Register8], modrm_index=0, constant_modrm=1),
    ASMInstruction("dec", [0x48], [ot.Register32], packed_register=True),

    ASMInstruction("inc", [0xFE], [ot.Mem8], modrm_index=0, constant_modrm=0),
    ASMInstruction("inc", [0xFF], [ot.Mem32], modrm_index=0, constant_modrm=0),

    ASMInstruction("or", [0x0A], [ot.Register8, ot.Register8], modrm_index=1),
    ASMInstruction("or", [0x0B], [ot.Register32, ot.Register32], modrm_index=1),

    ASMInstruction("cmp", [0x3A], [ot.Register8, ot.Mem8], modrm_index=1),
    ASMInstruction("cmp", [0x3B], [ot.Register32, ot.Mem32], modrm_index=1),

    ASMInstruction("shl", [0xD2], [ot.Mem8, ot.Register8], modrm_index=0, constant_modrm=4,
                   operand_restrictions=[None, "cl"]),
    ASMInstruction("shl", [0xD3], [ot.Mem32, ot.Register8], modrm_index=0, constant_modrm=4,
                   operand_restrictions=[None, "cl"]),

    ASMInstruction("in", [0xE4], [ot.Register8, ot.Constant8], operand_restrictions=["al", None], constant_imm=1),
    ASMInstruction("in", [0xE5], [ot.Register32, ot.Constant8], operand_restrictions=["eax", None], constant_imm=1),

    ASMInstruction("add", [0x80], [ot.Mem8, ot.Constant8], modrm_index=0, constant_modrm=0, constant_imm=1),
    ASMInstruction("add", [0x81], [ot.Mem32, ot.Constant32], modrm_index=0, constant_modrm=0, constant_imm=4),
    ASMInstruction("add", [0x83], [ot.Mem32, ot.Constant8], modrm_index=0, constant_modrm=0, constant_imm=1),

    ASMInstruction("jnz", [0x75], [ot.LabelBackward], constant_imm=1),
    ASMInstruction("jnz", [0x75], [ot.LabelForward], constant_imm=5),

    ASMInstruction("jmp", [0xE9], [ot.LabelBackward], constant_imm=4),
    ASMInstruction("jmp", [0xEB], [ot.LabelForward], constant_imm=4),
]
