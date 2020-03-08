using System.Collections.Generic;
using System.Linq;

namespace CourseWork.LexicalAnalysis
{
    public class Lexeme
    {
        public List<Token> Tokens { get; }

        public Lexeme(List<Token> tokens)
        {
            Tokens = tokens;
        }

        public override string ToString()
        {
            return $"[{string.Join(", ", Tokens.Select(p => "\"" + p.ToSourceValue()+ "\""))}]";
        }
    }
}