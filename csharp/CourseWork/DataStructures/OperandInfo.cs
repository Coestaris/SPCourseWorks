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

        public int GetByteCount()
        {
            var c = 0;
            switch (Type)
            {
                case OperandType.Register:
                    if (Token.Type == TokenType.Register8)
                        c = 1;
                    else if (Token.Type == TokenType.Register16)
                        c = 2;
                    else if (Token.Type == TokenType.Register32)
                        return 4;
                    break;

                case OperandType.Constant:
                    c = Token.ByteCount();
                    break;

                case OperandType.IndexedName:
                    break;
                case OperandType.Label:
                    break;
            }

            return c;
        }
    }
}