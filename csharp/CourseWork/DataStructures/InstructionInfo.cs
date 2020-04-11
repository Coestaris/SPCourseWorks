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

        private static readonly List<InstructionInfo> Infos = new List<InstructionInfo>
        {
            new InstructionInfo
            {
                Name = "movsb", OpCode1 = 0xA4, AllowedTypes = new List<OperandType>()
            },

            new InstructionInfo
            {
                Name = "pop", OpCode1 = 0x58, AllowedTypes = new List<OperandType> {OperandType.Register32},
                PackedRegister = true,
            },

            new InstructionInfo
            {
                Name = "dec", OpCode1 = 0xFE, AllowedTypes = new List<OperandType> {OperandType.IndexedName8},
                HasModRM = true, ConstantModRM = 1,
            },

            new InstructionInfo
            {
                Name = "dec", OpCode1 = 0xFF, AllowedTypes = new List<OperandType> {OperandType.IndexedName32},
                HasModRM = true, ConstantModRM = 1,
            },

            new InstructionInfo
            {
                Name = "add", OpCode1 = 0x2, AllowedTypes = new List<OperandType> {OperandType.Register8, OperandType.Register8},
                HasModRM = true,
            },

            new InstructionInfo
            {
                Name = "add", OpCode1 = 0x3, AllowedTypes = new List<OperandType> {OperandType.Register32, OperandType.Register32},
                HasModRM = true,
            },

            new InstructionInfo
            {
                Name = "cmp", OpCode1 = 0x3A, AllowedTypes = new List<OperandType> {OperandType.Register8, OperandType.IndexedName8},
                HasModRM = true,
            },

            new InstructionInfo
            {
                Name = "cmp", OpCode1 = 0x3B, AllowedTypes = new List<OperandType> {OperandType.Register32, OperandType.IndexedName32},
                HasModRM = true,
            },

            new InstructionInfo
            {
                Name = "bt", OpCode1 = 0x0F, OpCode2 = 0xA3, AllowedTypes = new List<OperandType> {OperandType.IndexedName32, OperandType.Register32},
                HasModRM = true,
            },

            new InstructionInfo
            {
                Name = "mov", OpCode1 = 0xB0, AllowedTypes = new List<OperandType> {OperandType.Register8, OperandType.Constant8},
                ConstantIMM = 1, PackedRegister = true,
            },

            new InstructionInfo
            {
                Name = "mov", OpCode1 = 0xB8, AllowedTypes = new List<OperandType> {OperandType.Register32, OperandType.Constant32},
                ConstantIMM = 4, PackedRegister = true,
            },


            new InstructionInfo
            {
                Name = "or", OpCode1 = 0x80, AllowedTypes = new List<OperandType> {OperandType.IndexedName8, OperandType.Constant8},
                HasModRM = true, ConstantModRM = 1, ConstantIMM = 1,
            },

            new InstructionInfo
            {
                Name = "or", OpCode1 = 0x83, AllowedTypes = new List<OperandType> {OperandType.IndexedName32, OperandType.Constant8},
                HasModRM = true, ConstantModRM = 1, ConstantIMM = 1,
            },

            new InstructionInfo
            {
                Name = "or", OpCode1 = 0x81, AllowedTypes = new List<OperandType> {OperandType.IndexedName32, OperandType.Constant32},
                HasModRM = true, ConstantModRM = 1, ConstantIMM = 4,
            },

            new InstructionInfo
            {
                Name = "jbe", AllowedTypes = new List<OperandType> {OperandType.Label},
            },
        };

        public override string ToString()
        {
            return $"{Name} (0x{OpCode1.ToString("X").PadRight(2, '0')}): {string.Join(", ", AllowedTypes)}";
        }

        public static int Match(Lexeme lexeme, out InstructionInfo info)
        {
            info = null;
            if (!lexeme.Structure.HasInstruction)
                return 1;

            var instruction = lexeme.Tokens[lexeme.Structure.InstructionIndex];
            var infos = Infos.FindAll(p => p.Name == instruction.StringValue);

            if (infos.Count == 0)
                return 1;

            foreach(var potentialInfo in infos)
            {
                if (potentialInfo.AllowedTypes.Count != lexeme.Structure.OperandInfos.Count)
                    continue;

                bool skip = false;
                for (var i = 0; i < potentialInfo.AllowedTypes.Count; i++)
                    if (potentialInfo.AllowedTypes[i] != lexeme.Structure.OperandInfos[i].Type)
                    {
                        // Assume that constant8 could be register constant32
                        if (lexeme.Structure.OperandInfos[i].Type == OperandType.Constant8 &&
                            potentialInfo.AllowedTypes[i] == OperandType.Constant32)
                        {
                            continue;
                        }

                        skip = true;
                        break;
                    }
                if(skip)
                    continue;

                info = potentialInfo;
                return 0;
            }

            return 2;
        }
    }
}