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


def second_pass(type, tokens, structure, storage, line_index):
    if type == LexemeType.var_def:
        get_var_bytes(tokens, storage, line_index)
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
