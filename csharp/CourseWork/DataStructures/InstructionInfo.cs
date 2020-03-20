using System.Collections.Generic;
using CourseWork.LexicalAnalysis;

namespace CourseWork.DataStructures
{
    public class InstructionInfo
    {
        public string Name;
        public List<OperandType> AllowedTypes;

        public static List<InstructionInfo> Infos = new List<InstructionInfo>
        {
            new InstructionInfo
                {Name = "pop", AllowedTypes = new List<OperandType> {OperandType.Register}},

            new InstructionInfo
                {Name = "dec", AllowedTypes = new List<OperandType> {OperandType.IndexedName}},

            new InstructionInfo
                {Name = "add", AllowedTypes = new List<OperandType> {OperandType.Register, OperandType.Register}},

            new InstructionInfo
                {Name = "cmp", AllowedTypes = new List<OperandType> {OperandType.Register, OperandType.IndexedName}},

            new InstructionInfo
                {Name = "bt", AllowedTypes = new List<OperandType> {OperandType.IndexedName, OperandType.Register}},

            new InstructionInfo
                {Name = "mov", AllowedTypes = new List<OperandType> {OperandType.Register, OperandType.IndexedName}},

            new InstructionInfo
                {Name = "or", AllowedTypes = new List<OperandType> {OperandType.IndexedName, OperandType.Constant}},

            new InstructionInfo
                {Name = "jbe", AllowedTypes = new List<OperandType> {OperandType.Label}},
        };

        public static bool Match(Lexeme lexeme)
        {
            if (!lexeme.Structure.HasInstruction)
                return false;

            var instruction = lexeme.Tokens[lexeme.Structure.InstructionIndex];
            var info = Infos.Find(p => p.Name == instruction.StringValue);

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