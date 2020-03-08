using CourseWork;
using CourseWork.DataStructures;
using CourseWork.LexicalAnalysis;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CourseWork
{
    public class Assembly
    {
        public AssemblySource Source { get; }
        public List<Lexeme> Lexemes { get; }
        public List<UserSegment> UserSegments { get; }
        public List<Token> UserLabels { get; }

        public Assembly(AssemblySource source)
        {
            Lexemes = new List<Lexeme>();
            UserSegments = new List<UserSegment>();
            UserLabels = new List<Token>();

            Source = source;
        }

        public Error Parse()
        {
            Tokenizer.Tokenize(this, out Error error);
            return error;
        }
    }
}
