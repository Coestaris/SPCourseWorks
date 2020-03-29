using System.Collections.Generic;
using CourseWork.LexicalAnalysis;

namespace CourseWork.DataStructures
{
    public class InstructionInfo
    {
        public string Name;
        public List<OperandType> AllowedTypes;

        public byte OpCode1;
        public byte OpCode2;

        public bool HasModRM;
        public byte ConstantModRM;

        public byte ConstantIMM;

        public bool PackedRegister;
        public bool PackedSizeBit;

        private static List<InstructionInfo> Infos = new List<InstructionInfo>
        {
            new InstructionInfo
            {
                OpCode1 = 0xA4,
                Name = "movsb", AllowedTypes = new List<OperandType>(),
            },

            new InstructionInfo
            {
                OpCode1 = 0x58,
                PackedRegister = true,
                Name = "pop", AllowedTypes = new List<OperandType> {OperandType.Register},
            },

            new InstructionInfo
            {
                OpCode1 = 0xFF,
                PackedSizeBit = true,
                HasModRM = true,
                ConstantModRM = 1,
                Name = "dec", AllowedTypes = new List<OperandType> {OperandType.IndexedName}
            },

            new InstructionInfo
            {
                OpCode1 = 0x3,
                PackedSizeBit = true,
                HasModRM = true,
                Name = "add", AllowedTypes = new List<OperandType> {OperandType.Register, OperandType.Register}
            },

            new InstructionInfo
            {
                OpCode1 = 0x3B,
                PackedSizeBit = true,
                HasModRM = true,
                Name = "cmp", AllowedTypes = new List<OperandType> {OperandType.Register, OperandType.IndexedName}
            },

            new InstructionInfo
            {
                OpCode1 = 0x0F,
                OpCode2 = 0xA3,
                HasModRM = true,
                Name = "bt", AllowedTypes = new List<OperandType> {OperandType.IndexedName, OperandType.Register}
            },

            new InstructionInfo
            {
                OpCode1 = 0xB8,
                ConstantIMM = 4,
                PackedRegister = true,
                Name = "mov", AllowedTypes = new List<OperandType> {OperandType.Register, OperandType.Constant}
            },

            new InstructionInfo
            {
                OpCode1 = 0x81,
                HasModRM = true,
                ConstantModRM = 1,
                ConstantIMM = 4,
                Name = "or", AllowedTypes = new List<OperandType> {OperandType.IndexedName, OperandType.Constant}
            },

            new InstructionInfo
            {
                OpCode1 = 0x76,
                ConstantIMM = 1,
                Name = "jbe", AllowedTypes = new List<OperandType> {OperandType.Label}
            },
        };

        public static bool Match(Lexeme lexeme, out InstructionInfo info)
        {
            info = null;
            if (!lexeme.Structure.HasInstruction)
                return false;

            var instruction = lexeme.Tokens[lexeme.Structure.InstructionIndex];
            info = Infos.Find(p => p.Name == instruction.StringValue);

            if (info == null)
                return false;

            if (info.AllowedTypes.Count != lexeme.Structure.OperandInfos.Count)
                return false;

            for(var i = 0; i < info.AllowedTypes.Count; i++)
                if (info.AllowedTypes[i] != lexeme.Structure.OperandInfos[i].Type)
                    return false;

            return true;
        }
    }
}