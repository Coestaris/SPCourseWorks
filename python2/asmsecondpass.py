from asmfirstpass import get_number
from asmtypes import *


def get_var_bytes(tokens, storage, line_index):
    bytes = Bytes()
    if tokens[1].value == "DB":
        if tokens[2].type == TokenType.string:
            bytes.set_imm_long([ord(x) for x in tokens[2].value.strip('\'')])
        else:
            bytes.set_imm8(get_number(tokens[2]))
    else:
        bytes.set_imm32(get_number(tokens[2]))
    storage.bytes[line_index] = bytes
    pass


def get_instruction_bytes(prototype_tuple, tokens, structure, storage, line_index):
    prot: InstructionPrototype = prototype_tuple[0]
    info: InstructionInfo = prototype_tuple[1]
    structure: LineStructure = structure

    bytes = Bytes()

    bytes.set_opcode(prot.opcode)
    if prot.packed:
        bytes.pack_reg(tokens[1].value)

    if prot.imm != 0:
        # IMM_INDEX always equals to 1, sooo ill just govnocode it....
        if prot.imm == 1:
            bytes.set_imm8(get_number(tokens[structure.op2_index]))
        else:
            bytes.set_imm32(get_number(tokens[structure.op2_index]))

    if prot.modrm != -1:

        if prot.modrm == 0:
            modrm_type = info.op1_type
            modrm_index = structure.op1_index
            nomodrm_type = info.op2_type
            nomodrm_index = structure.op2_index
        else:
            modrm_type = info.op2_type
            modrm_index = structure.op2_index
            nomodrm_type = info.op1_type
            nomodrm_index = structure.op1_index

        # Fill MOD field
        if prot.mc != -1:
            bytes.set_mod_const(prot.mc)
        else:
            if nomodrm_type == info.Register8 or nomodrm_type == info.Register32:
                bytes.set_mod_reg(tokens[nomodrm_index].value)

        # Fill RM field
        if modrm_type == info.Register8 or modrm_type == info.Register32:
            bytes.set_rm_reg(tokens[modrm_index].value)
        else:
            un = storage.get_user_name(False, info.memory_base.value)
            offset = storage.offsets[un.line][0]

            # Mem
            if info.direct_memory:
                bytes.set_rm_mem_direct(offset)
            else:
                bytes.set_rm_mem_index(offset, info.memory_index.value)

    if info.segment_change:
        if info.memory_index is not None:
            if info.segment_change.value == "SS":
                if info.memory_index.value != "EBP" and info.memory_index.value != "ESP":
                    bytes.set_segment_prefix(info.segment_change.value)
            elif info.segment_change.value == "DS":
                if info.memory_index.value == "EBP" or info.memory_index.value == "ESP":
                    bytes.set_segment_prefix(info.segment_change.value)
            else:
                bytes.set_segment_prefix(info.segment_change.value)
        else:
            if info.segment_change.value != "DS":
                bytes.set_segment_prefix(info.segment_change.value)

    storage.bytes[line_index] = bytes
    pass


def second_pass(type, prot, tokens, structure, storage, line_index, out_file):
    if type == LexemeType.var_def:
        get_var_bytes(tokens, storage, line_index)
    elif type == LexemeType.instruction:
        get_instruction_bytes(prot, tokens, structure, storage, line_index)
        if storage.bytes[line_index].get_size() != storage.offsets[line_index][1]:
            print("Unequal sizes. Expected {1}, got {0}".format(
                storage.bytes[line_index].get_size(),
                storage.offsets[line_index][1]
            ), file=out_file)
        pass
    pass


def print_line(line, storage, line_index, out_file):
    if storage.has_error(line_index):
        print("| {:2} |:        -----       ERROR       -----    :| {}".format(line_index + 1, line), file=out_file)
    else:
        if line_index in storage.offsets:
            offset = to_hex(storage.offsets[line_index][0])
            size = storage.offsets[line_index][1]
        else:
            offset = "   "
            size = ""

        if line_index in storage.bytes:
            bytes = str(storage.bytes[line_index])
        else:
            bytes = "---"

        print("| {:2} || {:>6} || {:>4} || {:21} || {}".format(line_index + 1, offset, size, bytes, line), file=out_file)
