package course.work.sp;

public enum TokenType {
    DbDir,
    DwDir,
    DdDir,

    Instruction,
    SegmentWord,
    EndWord,
    EndsWord,

    Reg8,
    Reg16,
    Reg32,

    Imm8,

    Symbol,

    Identifier,

    HexNumber,
    DecNumber,
    BinNumber,
    Text,
    PtrWord,
    ByteWord,
    DwordWord,


    UserSegment,
    SegmentRegister,
    Label,
    KeyWord,

    Unknown,
}
