using System;
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
                                operandInfo.Type = OperandType.Register8;
                                break;
                            case TokenType.Register16:
                            case TokenType.Register32:
                            case TokenType.Text:
                                operandInfo.Type = OperandType.Register32;
                                break;
                            case TokenType.BinNumber:
                            case TokenType.DecNumber:
                            case TokenType.HexNumber:
                            {
                                var value = operandInfo.Token.ToValue();
                                if(Math.Abs(value) <= 255)
                                    operandInfo.Type = OperandType.Constant8;
                                else
                                    operandInfo.Type = OperandType.Constant32;
                                break;
                            }

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

                    if(operandInfo.Token.Type != TokenType.Identifier)
                    {
                        return new Error(ErrorType.VariableExpected, operandInfo.Token);
                    }

                    if (operandInfo.SumOperand1.Type != TokenType.Register32 ||
                        operandInfo.SumOperand2.Type != TokenType.Register32)
                    {
                        return new Error(ErrorType.Register32Expected, operandInfo.SumOperand1);
                    }

                    if (operandInfo.OperandTokens[offset + 1].StringValue != "[" ||
                        operandInfo.OperandTokens[offset + 3].StringValue != "+" ||
                        operandInfo.OperandTokens[offset + 5].StringValue != "]")
                    {
                        return new Error(ErrorType.NotSupportedExpressionType, operandInfo.OperandTokens[offset]);
                    }

                    var variable =
                        ParentLexeme.ParentAssembly.UserVariables.Find(p =>
                            p.Name.StringValue == operandInfo.Token.StringValue);

                    if(variable == null)
                        return new Error(ErrorType.UndefinedReference, operandInfo.Token);

                    if(variable.Type.Type == TokenType.DbDirective)
                        operandInfo.Type = OperandType.IndexedName8;
                    else
                        operandInfo.Type = OperandType.IndexedName32;
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