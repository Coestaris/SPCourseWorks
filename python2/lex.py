class ASMInstruction:
    def __init__(self, name: str, opcode: list[int], operand_types: list[ot],
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