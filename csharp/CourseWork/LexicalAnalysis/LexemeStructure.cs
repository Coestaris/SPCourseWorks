using System.Collections.Generic;
using System.Linq;
using CourseWork.DataStructures;

namespace CourseWork.LexicalAnalysis
{
    public class LexemeStructure
    {
        public Lexeme ParentLexeme { get; private set; }

        public bool HasName { get; internal  set;}
        public bool HasInstruction { get; internal  set;}
        public bool HasOperands { get; internal  set;}

        public int InstructionIndex { get; internal  set;}
        public List<OperandInfo> OperandInfos { get; private set; }

        internal Error AnalyzeOperands()
        {
            if (HasInstruction && HasOperands &&
                ParentLexeme.Tokens[0].Type != TokenType.IfDirective &&
                (ParentLexeme.Tokens.Count == 2 || ParentLexeme.Tokens[1].Type != TokenType.EquDirective))
            {
                foreach (var operandInfo in OperandInfos)
                {
                    operandInfo.OperandTokens = ParentLexeme.Tokens
                        .Skip(operandInfo.Index)
                        .Take(operandInfo.Length)
                        .ToList();

                    // Register, constant or label
                    if (operandInfo.OperandTokens.Count == 1)
                    {
                        operandInfo.Token = operandInfo.OperandTokens[0];
                        switch (operandInfo.Token.Type)
                        {
                            case TokenType.Register8:
                            case TokenType.Register16:
                            case TokenType.Register32:
                            case TokenType.Text:
                                operandInfo.Type = OperandType.Register;
                                break;
                            case TokenType.BinNumber:
                            case TokenType.DecNumber:
                            case TokenType.HexNumber:
                                operandInfo.Type = OperandType.Constant;
                                break;
                            case TokenType.Label:
                                operandInfo.Type = OperandType.Label;
                                break;
                            default:
                                return new Error(ErrorType.WrongTokenAsOperand, operandInfo.Token);
                        }

                        continue;
                    }

                    if (operandInfo.OperandTokens.Count < 6)
                        return new Error(ErrorType.NotSupportedExpressionType, operandInfo.OperandTokens[0]);

                    operandInfo.Type = OperandType.IndexedName;
                    var offset = 0;
                    if (operandInfo.OperandTokens[0].Type == TokenType.SegmentRegister)
                    {
                        operandInfo.SegmentPrefix = operandInfo.OperandTokens[0];
                        offset += 2;

                        if (operandInfo.OperandTokens.Count != 8)
                            return new Error(ErrorType.NotSupportedExpressionType, operandInfo.OperandTokens[0]);
                    }

                    operandInfo.Token = operandInfo.OperandTokens[offset];
                    operandInfo.SumOperand1 = operandInfo.OperandTokens[offset + 2];
                    operandInfo.SumOperand2 = operandInfo.OperandTokens[offset + 4];

                    if (operandInfo.OperandTokens[offset + 1].StringValue != "[" &&
                        operandInfo.OperandTokens[offset + 3].StringValue != "+" &&
                        operandInfo.OperandTokens[offset + 5].StringValue != "]")
                    {
                        return new Error(ErrorType.NotSupportedExpressionType, operandInfo.OperandTokens[offset]);
                    }

                    var variable =
                        ParentLexeme.ParentAssembly.UserVariables.Find(p =>
                            p.Name.StringValue == operandInfo.Token.StringValue);
                    if(variable == null)
                        return new Error(ErrorType.UndefinedReference, operandInfo.Token);


                }
            }

            return null;
        }

        public string ToTable(int i = 0)
        {
            var res = "";
            if (HasName)
                res += $"{i,4} |";
            else
                res += $"{"--",4} |";

            if (HasInstruction)
                res += $"{i + InstructionIndex,4} {1,4} |";

            if(HasOperands && HasInstruction)
                for (var j = 0; j < OperandInfos.Count; j++)
                    res += $"{i + OperandInfos[j].Index,4} {OperandInfos[j].Length,4}{(j == OperandInfos.Count - 1 ? "" : " |")}";

            res += "\n";
            return res;
        }

        internal LexemeStructure(Lexeme parentLexeme)
        {
            ParentLexeme = parentLexeme;
            OperandInfos = new List<OperandInfo>();
        }
    }
}