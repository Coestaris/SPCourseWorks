namespace CourseWork
{
    public enum ErrorType
    {
        SameUserSegmentAlreadyExists,
        UnclosedQuotes,
        UnknownToken,
        SameLabelAlreadyExists,
        UserSegmentNamesMismatch,
        SpecifiedUserSegmentAlreadyClosed,
        UnclosedSegment,
        UnopenedSegment,
        MissingEndKeyword,
        NotSupportedExpressionType,
        WrongTokenAsOperand,
        WrongDirectiveFormat,
        ConstantExpected,
        UnexpectedDirective,
        SameEquAlreadyExists,
        SameVarAlreadyExists,
        InstructionFormat,
        WrongType,
        ExpectedNumberOrString,
        UndefinedReference,
        UnexpectedToken
    }
}