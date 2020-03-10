using CourseWork.DataStructures;
using System;
using System.Collections.Generic;
using System.Linq;

namespace CourseWork.LexicalAnalysis
{
    public class Lexeme
    {
        public bool HasLabel =>
            Tokens.Count >= 2 &&
            Tokens[0].Type == TokenType.Label;

        public Assembly ParrentAssembly { get; }
        public List<Token> Tokens { get; private set; }

        private void AssignUserSegmentsAndLabels(out Error error)
        {
            error = null;

            // *something* SEGMENT
            if (Tokens.Count == 2 &&
                Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.SegmentKeyword)
            {
                Tokens[0].Type = TokenType.UserSegment;
                if (ParrentAssembly.UserSegments.FindIndex(p => p.Name == Tokens[0].StringValue) != -1)
                {
                    error = new Error(ErrorType.SameUserSegmentAlreadyExists, Tokens[0]);
                    return;
                }

                ParrentAssembly.UserSegments.Add(new UserSegment() { 
                    OpenToken = Tokens[0], 
                });
            }

            // *something* ENDS
            if (Tokens.Count >= 2 &&
                Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.EndsKeyword)
            {
                Tokens[0].Type = TokenType.UserSegment;
                var structure = ParrentAssembly.UserSegments.Find(p => p.Name == Tokens[0].StringValue);

                // ≈сли не найдена структура которую мы собрались закрывать
                if(structure == null)
                {
                    error = new Error(ErrorType.UserSegmentNamesMismatch, Tokens[0]);
                    return;
                }
                
                // ≈сли структура которую мы собрались закрывать уже закрыта
                if (structure.Closed)
                {
                    error = new Error(ErrorType.SpecifiedUserSegmentAlreadyClosed, Tokens[0]);
                    return;
                }

                structure.CloseToken = Tokens[0];
            }

            // *something*:
            if (Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.Symbol && 
                Tokens[1].StringValue == ":")
            {
                Tokens[0].Type = TokenType.Label;
                if (ParrentAssembly.UserLabels.FindIndex(p => p.StringValue == Tokens[0].StringValue) != -1)
                {
                    error = new Error(ErrorType.SameLabelAreadyExists, Tokens[0]);
                    return;
                }
                ParrentAssembly.UserLabels.Add(Tokens[0]);
            }
        }

        private void AssignInlineUserSegmentsAndLabels(out Error error)
        {
            error = null;
            foreach(var token in Tokens)
                if(token.Type == TokenType.Identifier)
                {
                    var segment = ParrentAssembly.UserSegments.Find(p => p.Name == token.StringValue);
                    if (segment != null)
                    {
                        token.Type = TokenType.UserSegment;
                        continue;
                    }

                    var label = ParrentAssembly.UserLabels.Find(p => p.StringValue == token.StringValue);
                    if (label != null)
                    {
                        token.Type = TokenType.Label;
                        continue;
                    }
                }
        }

        public Lexeme(Assembly assembly) 
        {
            ParrentAssembly = assembly;
        }

        public void SetTokens(List<Token> tokens, out Error error)
        {
            error = null;
            Tokens = tokens;
            AssignUserSegmentsAndLabels(out error);
            if (error != null) return;

            // –аботает но суд€ по методе не нужно
            AssignInlineUserSegmentsAndLabels(out error);
            if (error != null) return;
        }

        public void ToSentenceTable(
            out int labelInd, out int instIndex, 
            out List<int> opIndices, out List<int> opLengths,
            out bool hasNoOperands, out bool hasNoInstruction)
        {
            instIndex = 0;
            hasNoInstruction = false;
            hasNoOperands = false;
            labelInd = -1;
            var offset = 0;
            if (HasLabel)
            {
                labelInd = 0;
                offset += 2; // label : ':'
            }

            opIndices = new List<int>();
            opLengths = new List<int>();

            if(Tokens.Count <= offset)
            {
                // has only label
                return;
            }

            hasNoInstruction = Tokens.Find(p =>
                       p.Type == TokenType.Instruction || p.IsDirective()) == null;

            if(hasNoInstruction)
            {
                // has no instructions
                return;
            }
            
            if (Tokens[offset].Type == TokenType.Identifier)
                offset += 1; // has name

            instIndex = offset;
            if (!hasNoInstruction)
                offset += 1;

            if (Tokens.Count <= offset)
            {
                // has only instrction (or + name)
                hasNoOperands = true;
                return;
            }

            var operand = 0;
            var i = 0;
            opIndices.Add(offset);
            opLengths.Add(0);
            
            foreach (var token in Tokens.Skip(offset))
            {
                if (token.Type == TokenType.Symbol && token.StringValue == ",")
                {
                    opIndices.Add(opLengths[operand]);
                    opLengths.Add(0);
                    operand++;
                }
                else
                {
                    opLengths[operand]++;
                }
                i++;
            }
        }

        public override string ToString()
        {
            return $"[{string.Join(", ", Tokens.Select(p => "\"" + p.ToString()+ "\""))}]";
        }

        public string ToTable()
        {
            return $"[{string.Join("", Tokens.Select(p => string.Format("{0,-30}", p.ToSourceValue(true)))).Trim()}]";
        }
    }
}