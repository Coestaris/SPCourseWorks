using System.Collections.Generic;

namespace CourseWork.LexicalAnalysis
{
    public class LexemeStructure
    {
        public bool HasName { get; internal  set;}
        public bool HasInstruction { get; internal  set;}
        public bool HasOperands { get; internal  set;}

        public int InstructionIndex { get; internal  set;}
        public List<int> OperandIndices { get; internal  set;}
        public List<int> OperandLengths { get; internal  set;}

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
                for (var j = 0; j < OperandIndices.Count; j++)
                    res += $"{i + OperandIndices[j],4} {OperandLengths[j],4}{(j == OperandIndices.Count - 1 ? "" : " |")}";

            res += "\n";
            return res;
        }

        internal LexemeStructure() {}
    }
}