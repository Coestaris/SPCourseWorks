package course.work.sp.tokenizer;

public enum TokenType {
    DbDir,
    DwDir,
    DdDir,
    DbIdentifier,
    DwIdentifier,
    DdIdentifier,
    SegmentIdentifier,

    Instruction,
    JmpWord,
    JncWord,
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

    BytePtr,
    DwordPtr,
    WordPtr,
    Ptr,

    SegmentRegister,
    KeyWord,
    Label,

    Mem8,
    Mem16,
    Mem32,
    Mem,

    Imm,

    Unknown,
    Empty,

    DS,
    CS,
    SS,
    ES,
    GS,
    FS

}
