using System;
using System.Collections.Generic;

namespace CourseWork.LexicalAnalysis
{
    public class OperandInfo
    {
        public List<Token> OperandTokens;

        public int Length;
        public int Index;

        public OperandType Type;

        // Register/Constant/Label fields
        public Token Token;

        // Indexed name fields
        public Token SegmentPrefix;
        public Token SumOperand1;
        public Token SumOperand2;
    }
}