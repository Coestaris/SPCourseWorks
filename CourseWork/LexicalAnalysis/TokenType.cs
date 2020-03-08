namespace CourseWork.LexicalAnalysis
{
    public enum TokenType
    {
        DbDirective,
        DwDirective,
        DdDirective,
        EquDirective,
        IfDirective,
        ElseDirective,
        EndifDirective,

        Instruction,
        SegmentKeyword,
        EndKeyword,
        EndsKeyword,

        Register32,
        Register16,
        Register8,

        Symbol,

        Identifier,

        HexNumber,
        DecNumber,
        BinNumber,
        Text,

        UserSegment,
        SegmentRegister,
        Label,
    }
}