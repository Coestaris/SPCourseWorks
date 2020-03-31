using System.Collections.Generic;
using CourseWork.LexicalAnalysis;

namespace kurs.DataStructures
{
    public class Equ
    {
        public Token Name { private set; get; }
        public List<Token> EquTokens { private set; get; }

        public Equ(Token name, List<Token> equTokens)
        {
            Name = name;
            EquTokens = equTokens;
        }
    }
}