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
            if(error != null) return error;
            
            foreach(var us in UserSegments)
            {
                if (!us.Closed)
                    return new Error(ErrorType.UnclosedSegment, us.OpenToken);
                if (!us.Opened)
                    return new Error(ErrorType.UnopenedSegment, us.CloseToken);
            }

            foreach(var lexeme in Lexemes)
            {
                lexeme.AssignInlineUserSegmentsAndLabels(out error);
                if (error != null) return error;
            }

            var lastLexeme = Lexemes.Last();
            if(lastLexeme.Tokens.Count != 1 ||
                lastLexeme.Tokens[0].Type != TokenType.EndKeyword)
            {
                return new Error(ErrorType.MissingEndKeyword);
            }

            return null;
        }
    }
}
