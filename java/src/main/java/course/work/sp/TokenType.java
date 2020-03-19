package course.work.sp;

public enum TokenType {
    DbDir,
    DwDir,
    DdDir,

    Instruction,
    JmpWord,
    SegmentWord,
    EndWord,
    EndsWord,

    Reg8,
    Reg16,
    Reg32,

    Symbol,
    Comma,

    Identifier,

    HexNumber,
    DecNumber,
    BinNumber,
    Text,
    PtrWord,
    ByteWord,
    DwordWord,

    SegmentRegister,
    KeyWord,

    Unknown,
}
