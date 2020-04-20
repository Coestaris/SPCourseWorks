package course.work.sp.tokenizer;

public enum TokenType {
    DbDir,
    DwDir,
    DdDir,
    DbIdentifier,
    DwIdentifier,
    DdIdentifier,
    SegmentIdentifier,

    //
    Db,
    Dw,  // must delete after newFirstPass
    Dd,
    //

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
    Mem,

    Imm,

    Unknown,

    DS,
    CS,
    SS,
    ES,
    GS,
    FS

}
