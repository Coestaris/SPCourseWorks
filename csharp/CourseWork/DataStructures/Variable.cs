using CourseWork.LexicalAnalysis;

namespace CourseWork.DataStructures
{
    public class Variable
    {
        public Token Name;
        public Token Type;
        public Token InitValue;

        public Variable(Token name, Token type, Token initValue)
        {
            Name = name;
            Type = type;
            InitValue = initValue;
        }
    }
}