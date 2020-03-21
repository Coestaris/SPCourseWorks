using System.Collections.Generic;
using System.Linq;
using CourseWork.DataStructures;

namespace CourseWork.LexicalAnalysis
{
    public class Lexeme
    {
        public bool HasLabel => Tokens.Count >= 2 && Tokens[0].Type == TokenType.Label;

        public bool HasOffset => Tokens.Count >= 1 && Tokens[0].Type == TokenType.Instruction ||
                                 Tokens.Count == 3 && Tokens[1].IsDirective() && Tokens[1].Type != TokenType.EquDirective;

        public Assembly ParentAssembly { get; }

        public List<Token> Tokens { get; private set; }

        public LexemeStructure Structure { get; private set; }

        public UserSegment Segment { get; internal set; }

        public InstructionInfo InstructionInfo { get; internal set; }

        public int Offset { get; internal set; }

        private void AssignUserSegmentsAndLabels(out Error error)
        {
            error = null;

            // *something* SEGMENT
            if (Tokens.Count == 2 &&
                Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.SegmentKeyword)
            {
                Tokens[0].Type = TokenType.UserSegment;
                if (ParentAssembly.UserSegments.FindIndex(p => p.Name == Tokens[0].StringValue) != -1)
                {
                    error = new Error(ErrorType.SameUserSegmentAlreadyExists, Tokens[0]);
                    return;
                }

                ParentAssembly.UserSegments.Add(new UserSegment
                {
                    OpenToken = Tokens[0],
                    Index = ParentAssembly.UserSegments.Count
                });
            }

            // *something* ENDS
            if (Tokens.Count >= 2 &&
                Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.EndsKeyword)
            {
                Tokens[0].Type = TokenType.UserSegment;
                var structure = ParentAssembly.UserSegments.Find(p => p.Name == Tokens[0].StringValue);

                if(structure == null)
                {
                    error = new Error(ErrorType.UserSegmentNamesMismatch, Tokens[0]);
                    return;
                }
                
                if (structure.Closed)
                {
                    error = new Error(ErrorType.SpecifiedUserSegmentAlreadyClosed, Tokens[0]);
                    return;
                }

                structure.CloseToken = Tokens[0];
            }

            // *something*:
            if (Tokens.Count >= 2 &&
                Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.Symbol && 
                Tokens[1].StringValue == ":")
            {
                Tokens[0].Type = TokenType.Label;
                if (ParentAssembly.UserLabels.FindIndex(p => p.StringValue == Tokens[0].StringValue) != -1)
                {
                    error = new Error(ErrorType.SameLabelAlreadyExists, Tokens[0]);
                    return;
                }
                ParentAssembly.UserLabels.Add(Tokens[0]);
            }

            // *something* equ *something*
            if (Tokens.Count == 3 &&
                Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.EquDirective)
            {
                if (ParentAssembly.Equs.ContainsKey(Tokens[0].StringValue))
                {
                    error = new Error(ErrorType.SameEquAlreadyExists, Tokens[0]);
                    return;
                }

                ParentAssembly.Equs.Add(Tokens[0].StringValue, Tokens[2]);
            }

            // *something* db/dw/dd *something*
            if (Tokens.Count == 3 &&
                Tokens[0].Type == TokenType.Identifier &&
                (Tokens[1].Type == TokenType.DbDirective ||
                 Tokens[1].Type == TokenType.DwDirective ||
                 Tokens[1].Type == TokenType.DdDirective))
            {
                if (ParentAssembly.UserVariables.Find(p => p.Name.StringValue == Tokens[2].StringValue) != null)
                {
                    error = new Error(ErrorType.SameVarAlreadyExists, Tokens[0]);
                    return;
                }

                ParentAssembly.UserVariables.Add(new Variable(Tokens[0], Tokens[1], Tokens[2]));
            }
        }

        internal void AssignInlineUserSegmentsAndLabels(out Error error)
        {
            error = null;
            foreach(var token in Tokens)
                if(token.Type == TokenType.Identifier)
                {
                    var segment = ParentAssembly.UserSegments.Find(p => p.Name == token.StringValue);
                    if (segment != null)
                    {
                        token.Type = TokenType.UserSegment;
                        continue;
                    }

                    var label = ParentAssembly.UserLabels.Find(p => p.StringValue == token.StringValue);
                    if (label != null)
                    {
                        token.Type = TokenType.Label;
                    }
                }
        }

        public Lexeme(Assembly assembly) 
        {
            ParentAssembly = assembly;
        }

        public void SetTokens(List<Token> tokens, out Error error)
        {
            error = null;
            Tokens = tokens;
            AssignUserSegmentsAndLabels(out error);
            if (error != null) return;

            Structure = GetStructure();
        }

        private LexemeStructure GetStructure()
        {
            var structure = new LexemeStructure(this)
            {
                InstructionIndex = 0,
                HasInstruction = true,
                HasOperands = true
            };

            var offset = 0;
            if (HasLabel)
            {
                structure.HasName = true;
                offset += 2; // label : ':'
            }

            if(Tokens.Count <= offset)
            {
                // has only label
                structure.HasInstruction = false;
                return structure;
            }

            if (Tokens[offset].Type == TokenType.Identifier || Tokens[offset].Type == TokenType.UserSegment)
            {
                // has name
                structure.HasName = true;
                offset += 1;
            }

            structure.HasInstruction = Tokens.Find(p =>
                       p.Type == TokenType.Instruction || p.IsDirective() ||
                       p.Type == TokenType.SegmentKeyword ||
                       p.Type == TokenType.EndsKeyword ||
                       p.Type == TokenType.EndKeyword) != null;

            if(!structure.HasInstruction)
            {
                // has no instructions
                return structure;
            }

            structure.InstructionIndex = offset;
            offset += 1;

            if (Tokens.Count <= offset)
            {
                // has only instruction (or + name)
                structure.HasOperands = false;
                return structure;
            }

            var operand = 0;
            structure.OperandInfos.Add(new OperandInfo {Index = offset});
            foreach (var token in Tokens.Skip(offset))
            {
                if (token.Type == TokenType.Symbol && token.StringValue == ",")
                {
                    structure.OperandInfos.Add(new OperandInfo
                    {
                        Index = structure.OperandInfos[operand].Length + operand + 1 + offset,
                        Length = 0
                    });
                    operand++;
                }
                else
                {
                    structure.OperandInfos[operand].Length++;
                }
            }

            return structure;
        }

        public override string ToString()
        {
            return $"[{string.Join(", ", Tokens.Select(p => "\"" + p.ToString()+ "\""))}]";
        }

        public string ToTable(bool ded)
        {
            if (ded)
                return string.Join(" ", Tokens.Select(p => p.Type == TokenType.Text ? "\"" + p.StringValue + "\"" :  p.StringValue));
            return $"[{string.Join("", Tokens.Select(p => string.Format("{0,-30}", p.ToSourceValue(true)))).Trim()}]";
        }

        public void ProceedEqu()
        {
            for (var i = 0; i < Tokens.Count; i++)
                if (ParentAssembly.Equs.ContainsKey(Tokens[i].StringValue))
                {
                    if(Tokens.Count == 3 && Tokens[1].Type == TokenType.EquDirective)
                        continue;

                    Tokens[i] = ParentAssembly.Equs[Tokens[i].StringValue];
                }
        }

        public int GetSize()
        {
            if (Structure.HasInstruction)
            {
                var instruction = Tokens[Structure.InstructionIndex];
                if (instruction.Type == TokenType.Instruction)
                {
                    // Regular instruction
                    var size = 1; // OpCode
                    if (InstructionInfo.OpCode2 != 0) size += 1; // 2 byte opCode

                    if (InstructionInfo.HasModRM) size += 1; // Has ModRM byte
                    if (InstructionInfo.ConstantIMM != 0) size += InstructionInfo.ConstantIMM;

                    if (InstructionInfo.AllowedTypes.Contains(OperandType.IndexedName))
                    {
                        size += 1; // SIB byte always exists when using base indexed
                        size += 4; // + DSIP32 (govnocoded =3)
                    }

                    foreach (var operandInfo in Structure.OperandInfos)
                    {
                        if (operandInfo.SegmentPrefix != null)
                            size += 1; // явная замена сегмента

                        if (operandInfo.Type == OperandType.IndexedName)
                        {
                            // Get variable
                            var a = ParentAssembly.UserVariables.Find(p =>
                                p.Name.StringValue == operandInfo.Token.StringValue);

                            // If variable not in data segment - неявная замена сегмента
                            if (a.Name.ParentLexeme.Segment.Index != 0)
                                size += 1;
                        }
                    }

                    return size;
                }
                else
                {
                    // Memory directive
                    if (instruction.Type == TokenType.DbDirective)
                    {
                        // Could be string
                        var operand = Structure.OperandInfos[0].Token;
                        if (operand.Type == TokenType.Text)
                            return operand.StringValue.Length;

                        return 1;
                    }


                    if (instruction.Type == TokenType.DwDirective)
                    {
                        return 2;
                    }

                    if (instruction.Type == TokenType.DdDirective)
                    {
                        return 4;
                    }

                    // Some other directive
                    return 0;
                }
            }

            return 0;
        }
    }
}