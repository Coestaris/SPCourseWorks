using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using CourseWork.Bytes;
using CourseWork.DataStructures;
using kurs.DataStructures;

namespace CourseWork.LexicalAnalysis
{
    public class Lexeme
    {
        public bool HasLabel => Tokens.Count >= 2 && Tokens[0].Type == TokenType.Label;

        public bool HasOffset => Tokens.Count >= 1 && Tokens[0].Type == TokenType.Instruction ||
                                 Tokens.Count == 3 && Tokens[1].IsDirective() && Tokens[1].Type != TokenType.EquDirective;

        public Assembly ParentAssembly { get; }
        public List<Token> Tokens { get; private set; }
        public LexemeStructure Structure { get; set; }
        public ByteStorage Storage { get; private set; }
        public UserSegment Segment { get; internal set; }
        public InstructionInfo InstructionInfo { get; internal set; }
        public int Offset { get; internal set; }
        public int Size { get; internal set; }
        public Error Error { get; set; }

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
            if (Tokens.Count >= 3 &&
                Tokens[0].Type == TokenType.Identifier &&
                Tokens[1].Type == TokenType.EquDirective)
            {
                if (ParentAssembly.UserEqus.FindIndex(p => p.Name.StringValue == Tokens[0].StringValue) != -1)
                {
                    error = new Error(ErrorType.SameEquAlreadyExists, Tokens[0]);
                    return;
                }

                ParentAssembly.UserEqus.Add(new Equ(Tokens[0], Tokens.Skip(2).ToList()));
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

                if(!Tokens[2].IsNumber() && Tokens[2].Type != TokenType.Text)
                {
                    error = new Error(ErrorType.ExpectedNumberOrString, Tokens[2]);
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

        public void InsertTokens(List<Token> tokens, out Error error)
        {
            error = null;
            Tokens = tokens;
            AssignUserSegmentsAndLabels(out error);
            if (error != null) return;
        }

        public LexemeStructure GetStructure()
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
            {
                Equ equ;
                if ((equ = ParentAssembly.UserEqus.Find(p => p.Name.StringValue == Tokens[i].StringValue)) != null)
                {
                    if (Tokens.Count >= 3 && Tokens[1].Type == TokenType.EquDirective)
                        continue;

                    Tokens.RemoveAt(i);
                    Tokens.InsertRange(i, equ.EquTokens);
                }
            }
        }

        public void GetBytes()
        {
            if (Error != null) return;
            if (!Structure.HasInstruction) return;
            if (Size == 0) return;

            Storage = new ByteStorage();

            var instruction = Tokens[Structure.InstructionIndex];
            if (instruction.Type == TokenType.Instruction)
            {
                // Hardcode JBE instruction because its weird....
                if (InstructionInfo.Name == "jbe")
                {
                    var labelOperand = Structure.OperandInfos[0].Token;
                    var labelLexeme =
                        ParentAssembly.UserLabels.Find(p => p.StringValue == labelOperand.StringValue);

                    var diff = labelLexeme.ParentLexeme.Offset - Offset - Size;

                    var forward = labelLexeme.Line > labelOperand.Line;
                    var far = Math.Abs(diff) >= 127;

                    if (!far && !forward)
                    {
                        Storage.SetOpcode(0x76);
                        Storage.SetIMM(ModRMSIB.ToBytes8(diff));
                        return;
                    }

                    if (far)
                    {
                        Storage.SetExpPrefix();
                        Storage.SetOpcode(0x86);
                        Storage.SetIMM(ModRMSIB.ToBytes32(diff));
                        return;
                    }

                    Storage.SetOpcode(0x76);
                    var imm = ModRMSIB.ToBytes8(diff + 4).ToList();
                    imm.AddRange(new List<byte> {0x90, 0x90, 0x90, 0x90});
                    Storage.SetIMM(imm.ToArray());
                    return;
                }


                // OPCODE
                byte opcode;
                if (InstructionInfo.OpCode2 != 0)
                {
                    Storage.SetExpPrefix();
                    opcode = InstructionInfo.OpCode2;
                }
                else
                {
                    opcode = InstructionInfo.OpCode1;
                }

                Storage.SetOpcode(opcode);

                if(InstructionInfo.PackedRegister)
                    Storage.PackRegister(Structure.OperandInfos[0].Token);

                // MODRM / SIB
                if (InstructionInfo.HasModRM)
                {
                    // REG FIELD
                    if(InstructionInfo.ConstantModRM != 0xFF)
                        Storage.SetReg(InstructionInfo.ConstantModRM);
                    else if (Structure.OperandInfos.Count != 1)
                    {
                        var dest = Structure.OperandInfos[InstructionInfo.SourceIndex == 0 ? 1 : 0];
                        if (dest.Type == OperandType.Register8 || dest.Type == OperandType.Register32)
                            Storage.SetReg(dest.Token);
                    }

                    // MODRM FIELD
                    var source = Structure.OperandInfos[InstructionInfo.SourceIndex];
                    if (source.Type == OperandType.Register8 || source.Type == OperandType.Register32)
                    {
                        // REGISTER
                        Storage.SetRM(source.Token);
                    }
                    else
                    {
                        // MEM
                        var variable =
                            ParentAssembly.UserVariables.Find(p => p.Name.StringValue == source.Token.StringValue);

                        // Used Variable from another segment. Switch it
                        if(variable.Name.ParentLexeme.Segment.OpenToken.Line == Segment.OpenToken.Line)
                            Storage.SetSegmentPrefix("cs");


                        Storage.SetRM(source.SumOperand1, source.SumOperand2, variable.Name.ParentLexeme.Offset);
                    }
                }

                // IMM / SEG PREFIX
                var i = 0;
                foreach (var info in Structure.OperandInfos)
                {
                    if (info.Type == OperandType.Constant8 || info.Type == OperandType.Constant32)
                    {
                        // We only care about stuff required by instruction
                        if(InstructionInfo.AllowedTypes[i] == OperandType.Constant8)
                            Storage.SetIMM(ModRMSIB.ToBytes8(info.Token.ToValue()));
                        else
                            Storage.SetIMM(ModRMSIB.ToBytes32(info.Token.ToValue()));
                    }

                    if(info.SegmentPrefix != null)
                        Storage.SetSegmentPrefix(info.SegmentPrefix);

                    i++;
                }
            }

            var valueToken = Structure.HasOperands ? Structure.OperandInfos[0].Token : null;
            // Memory directive

            switch (instruction.Type)
            {
                // Could be string
                case TokenType.DbDirective when valueToken.Type == TokenType.Text:
                    Storage.SetIMM(valueToken.StringValue.Select(p => (byte)p).ToArray());
                    return;

                case TokenType.DbDirective:
                {
                    var value = valueToken.ToValue();
                    Storage.SetIMM(ModRMSIB.ToBytes8(value));
                    break;
                }

                case TokenType.DwDirective:
                {
                    var value = valueToken.ToValue();
                    Storage.SetIMM(ModRMSIB.ToBytes16(value));
                    break;
                }

                case TokenType.DdDirective:
                {
                    var value = valueToken.ToValue();
                    Storage.SetIMM(ModRMSIB.ToBytes32(value));
                    break;
                }
            }
        }

        public int GetSize()
        {
            if (Error != null) return 0;

            if (!Structure.HasInstruction) return 0;

            var instruction = Tokens[Structure.InstructionIndex];
            if (instruction.Type == TokenType.Instruction)
            {
                if (InstructionInfo.Name == "jbe")
                {
                    var labelOperand = Structure.OperandInfos[0].Token;
                    var labelLexeme =
                        ParentAssembly.UserLabels.Find(p => p.StringValue == labelOperand.StringValue);

                    var diff = labelLexeme.ParentLexeme.Offset - Offset;

                    var forward = labelLexeme.Line > labelOperand.Line;
                    var far = Math.Abs(diff) >= 127;

                    if (!far && !forward)
                        return 2;

                    return 6;
                }

                var size = 0;

                // OPCODE
                size++;
                // EXP PREFIX
                if (InstructionInfo.OpCode2 != 0)
                    size++;

                // MODRM BYTE
                if (InstructionInfo.HasModRM)
                    size++;

                // IMM FIELD
                if (InstructionInfo.ConstantIMM != 0)
                    size += InstructionInfo.ConstantIMM;

                // SEG PREFIX / SIB + DISP32
                foreach (var operandInfo in Structure.OperandInfos)
                {
                    if (operandInfo.Type == OperandType.IndexedName8 ||
                        operandInfo.Type == OperandType.IndexedName32)
                    {
                        // Check for segment automatic change
                        var variable =
                            ParentAssembly.UserVariables.Find(p => p.Name.StringValue == operandInfo.Token.StringValue);

                        if (variable.Name.ParentLexeme.Segment.OpenToken.Line == Segment.OpenToken.Line)
                            size++;

                        size++; // SIB
                        size += 4; // DISP32
                    }

                    if (operandInfo.SegmentPrefix != null && operandInfo.SegmentPrefix.StringValue != "ds")
                        size++; // SEG PREFIX
                }

                return size;
            }

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
}