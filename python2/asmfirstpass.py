from asmtypes import LineStructure, Storage, Token, LexemeType, TokenType, UserName, InstructionInfo, \
    InstructionPrototype


def print_segments(storage, out_file):
    print("="*40, file=out_file)
    print("|   NAME   |    SIZE   |  LINE |   SIZE   |", file=out_file)
    print("="*40, file=out_file)
    print("|     DATA |        32 |    {} |    {}    |".format(0, 0), file=out_file)
    print("|     CODE |        32 |    {} |    {}    |".format(0, 0), file=out_file)
    print("="*40, file=out_file)
    pass


def print_segment_destinations(storage, out_file):
    print("="*40, file=out_file)
    print("|   SEGMENT REGISTER   |  DESTINATION  |", file=out_file)
    print("="*40, file=out_file)
    print("|                 CS   |       CODE    |", file=out_file)
    print("|                 DS   |       DATA    |", file=out_file)
    print("|                 FS   |    NOTHING    |", file=out_file)
    print("|                 GS   |    NOTHING    |", file=out_file)
    print("|                 SS   |    NOTHING    |", file=out_file)
    print("|                 ES   |    NOTHING    |", file=out_file)
    print("="*40, file=out_file)
    pass


def print_user_names(storage, out_file):
    print("="*40, file=out_file)
    print("|   NAME   |    TYPE   |  LINE |   VALUE   |", file=out_file)
    print("|     DATA |   SEGMENT |    {} |   -----   |".format(0), file=out_file)
    print("|     CODE |   SEGMENT |    {} |   -----   |".format(0), file=out_file)
    pass


prototypes = []


# FD std 1
#
# push mem
# FF /6 PUSH r/m16
# FF /6 PUSH r/m32
#
# pop reg
# 58+rd POP r32
#
# idiv reg, reg
# F6 /7 IDIV AL, r/m8
# F7 /7 IDIV EAX, r/m32
#
# add reg, mem
# 02 /r ADD r8, r/m8
# 03 /r ADD r32, r/m32
#
# adc mem, reg
# 10 /r Adc r/m8, r8
# 11 /r Adc r/m32, r32
#
# in reg, imm
# E4 ib IN AL, imm8
# E5 ib IN EAX, imm8
#
# or mem,imm
# 80 /1 ib OR r/m8, imm8
# 81 /1 id OR r/m32, imm32
# 83 /1 ib OR r/m32, imm8
#
# jnge
#
# jmp
def init_prototypes():
    global prototypes
    i = InstructionInfo
    prototypes = [
        InstructionPrototype("std", 0xFD),

        InstructionPrototype("push", 0xFF, 1, i.Memory32, modrm=True, mc=6),

        InstructionPrototype("pop", 0x58, 1, i.Register32, packed=True),

        InstructionPrototype("idiv", 0xF6, 1, i.Register8, i.Register8, modrm=True, mc=7),
        InstructionPrototype("idiv", 0xF7, 1, i.Register32, i.Register32, modrm=True, mc=7),

        InstructionPrototype("add", 0x02, 2, i.Register8, i.Memory8, modrm=True),
        InstructionPrototype("add", 0x03, 2, i.Register32, i.Memory32, modrm=True),

        InstructionPrototype("adc", 0x10, 2, i.Memory8, i.Register8, modrm=True),
        InstructionPrototype("adc", 0x11, 2, i.Memory32, i.Register32, modrm=True),

        InstructionPrototype("in", 0xE4, 2, i.Register8, i.IMM8, imm=1),
        InstructionPrototype("in", 0xE5, 2, i.Register32, i.IMM8, imm=4),
        InstructionPrototype("in", 0xE5, 2, i.Register32, i.IMM32, imm=4),

        InstructionPrototype("or", 0x80, 2, i.Memory8, i.IMM8, modrm=True, mc=1, imm=1),
        InstructionPrototype("or", 0x81, 2, i.Memory32, i.IMM32, modrm=True, mc=1, imm=4),
        InstructionPrototype("or", 0x83, 2, i.Memory32, i.IMM8, modrm=True, mc=1, imm=1),

        InstructionPrototype("jnge", 0x00, 1),
        InstructionPrototype("jnge", 0x00, 1),

        InstructionPrototype("jmp", 0x00, 1),
        InstructionPrototype("jmp", 0x00, 1),
    ]
    pass


def get_number(token):
    if token.type == TokenType.bin_number:
        return int(token.value[:-1], base=2)
    elif token.type == TokenType.dec_number:
        return int(token.value)


def get_lexeme_type(tokens, structure):
    if len(tokens) == 0:
        return LexemeType.blank

    if len(tokens) == 2 and tokens[0].value == ".MODEL" and tokens[1].type == TokenType.model:
        return LexemeType.model

    if len(tokens) == 1 and tokens[0].value == ".DATA":
        return LexemeType.data_seg

    if len(tokens) == 1 and tokens[0].value == ".CODE":
        return LexemeType.code_seg

    if len(tokens) == 1 and tokens[0].value == "END":
        return LexemeType.end

    if len(tokens) == 2 and tokens[0].type == TokenType.user_type and tokens[1].value == ":":
        return LexemeType.label

    if len(tokens) >= 1 and tokens[0].type == TokenType.instruction:
        return LexemeType.instruction

    if len(tokens) == 3 and tokens[0].type == TokenType.user_type and \
            (tokens[1].value == "DB" or tokens[1].value == "DD") and \
            (tokens[2].type == TokenType.bin_number or tokens[2].type == TokenType.dec_number or tokens[
                2].type == TokenType.string):
        return LexemeType.var_def

    return LexemeType.unknown


def create_instruction_info(tokens, structure, storage, line_index):
    info = InstructionInfo()
    for i, op_id in enumerate(structure.get_ops()):
        op_type = 0
        op = tokens[op_id[0]: op_id[0] + op_id[1]]
        if len(op) == 1:
            # Reg, Constant or label
            if op[0].type == TokenType.register8:
                op_type = InstructionInfo.Register8
            elif op[0].type == TokenType.register32:
                op_type = InstructionInfo.Register32
            elif op[0].type == TokenType.bin_number or op[0].type == TokenType.dec_number:
                number = get_number(op[0])
                if abs(number) > 0xFFFFFF:
                    storage.set_error(line_index, "Constant is too big ")
                    return None
                if abs(number) > 0xFF:
                    op_type = InstructionInfo.IMM32
                else:
                    op_type = InstructionInfo.IMM8
            elif op[0].type == TokenType.user_type:
                # Direct memory or label
                # Assume that variables are always defined
                un = storage.get_user_name(False, op[0].value)
                if un is None:
                    # No variable found - its a label
                    un = storage.get_user_name(True, op[0].value)
                    if un is None:
                        op_type = InstructionInfo.LabelForward
                    else:
                        op_type = InstructionInfo.LabelBackward
                else:
                    info.direct_memory = True
                    info.memory_base = op[0]

                    if un.type == "DB":
                        op_type = InstructionInfo.Memory8
                    else:
                        op_type = InstructionInfo.Memory32
        else:
            # SEG : NAME case
            if len(op) == 3 and op[0].type == TokenType.register_seg and op[1].value == ":" and op[
                2].type == TokenType.user_type:
                un = storage.get_user_name(False, op[2].value)
                if un is None:
                    storage.set_error(line_index, "Undefined variable reference")
                    return None
                info.direct_memory = True
                info.memory_base = op[2]
                if un.type == "DB":
                    op_type = InstructionInfo.Memory8
                else:
                    op_type = InstructionInfo.Memory32
            else:
                # Indirect memory
                if len(op) < 4 or len(op) > 6:
                    storage.set_error(line_index, "Invalid token count im memory operand")
                    return None

                offset = 0
                if op[0].type == TokenType.register_seg:
                    offset = 2
                    if op[1].value != ":":
                        storage.set_error(line_index, "':' symbol expected")
                        return None
                    info.segment_change = op[0]

                if len(op) - offset != 4:
                    storage.set_error(line_index, "Invalid token count im memory operand")
                    return None

                if op[offset].type != TokenType.user_type or \
                        op[offset + 1].value != "[" or \
                        op[offset + 2].type != TokenType.register32 or \
                        op[offset + 3].value != "]":
                    storage.set_error(line_index, "Invalid memory format")
                    return None

                info.direct_memory = False
                info.memory_base = op[offset]
                info.memory_index = op[offset + 2]
                if info.memory_index.value == "ESP":
                    storage.set_error(line_index, "Invalid index register")
                    return None

                un = storage.get_user_name(False, op[offset].value)
                if un is None:
                    storage.set_error(line_index, "Undefined variable reference")
                    return None
                if un.type == "DB":
                    op_type = InstructionInfo.Memory8
                else:
                    op_type = InstructionInfo.Memory32

        if i == 0:
            info.op1_type = op_type
        else:
            info.op2_type = op_type
    return info


def first_pass(tokens, structure, storage, line_index):
    type = get_lexeme_type(tokens, structure)

    if type == LexemeType.unknown:
        storage.set_error(line_index, "Unknown lexeme type")
        return False

    if type == LexemeType.model:
        if storage.model_line != -1:
            storage.set_error(line_index, "Model type already declared at line " + str(storage.model_line + 1))
            return False
        storage.model_line = line_index

    elif type == LexemeType.data_seg:
        if storage.data_start != -1:
            storage.set_error(line_index, "Data segment already declared at line " + str(storage.data_start + 1))
            return False
        storage.data_start = line_index

    elif type == LexemeType.code_seg:
        if storage.code_start != -1:
            storage.set_error(line_index, "Code segment already declared at line " + str(storage.code_start + 1))
            return False
        storage.code_start = line_index

    elif type == LexemeType.end:
        pass  # idk

    elif type == LexemeType.blank:
        pass  # idk

    else:
        segment = storage.get_segment(line_index)

        if segment == Storage.NoSegment:
            storage.set_error(line_index, "You should declare segment first")
            return False

        if type == LexemeType.var_def:
            un = storage.get_user_name(False, tokens[0].value)
            if un is not None:
                storage.set_error(line_index, "Same variable already declared at line" + str(un.line + 1))
                return False

            if tokens[2].type == TokenType.string and tokens[1].value != "DB":
                storage.set_error(line_index, "You can declare string only with DB directive")
                return False

            # Check variable range
            if tokens[2].type != TokenType.string:
                value = get_number(tokens[2])
                if tokens[1].value == "DB" and abs(value) > 0xFF:
                    storage.set_error(line_index, "Constant is too large for 8bit variable")
                    return False
                if tokens[1].value == "DD" and abs(value) > 0xFFFFFF:
                    storage.set_error(line_index, "Constant is too large for 8bit variable")
                    return False

            un = UserName(tokens[0].value, False, tokens[1].value, line_index)
            storage.user_names.append(un)

        elif type == LexemeType.label:
            if segment != Storage.CodeSegment:
                storage.set_error(line_index, "You can declare labels only in CODE segment")
                return False

            un = storage.get_user_name(True, tokens[0].value)
            if un is not None:
                storage.set_error(line_index, "Same label already declared at line" + str(un.line + 1))
                return False

            un = UserName(tokens[0].value, True, None, line_index)
            storage.user_names.append(un)

        else:
            # Instruction
            if segment != Storage.CodeSegment:
                storage.set_error(line_index, "You can declare instructions only in CODE segment")
                return False

            info = create_instruction_info(tokens, structure, storage, line_index)
            if info is None:
                return False

    return True


def print_line(line, out_file):
    print(line, file=out_file)
