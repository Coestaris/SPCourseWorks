package course.work.sp.tokenizer;

public enum TokenType {
    DbDir,
    DwDir,
    DdDir,
    Db,
    Dw,
    Dd,

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
    IdentifierEnd,


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

    Imm8,
    Imm16,
    Imm32,

    Unknown,

    DS,
    CS,
    SS,
    ES,
    GS,
    FS

}
