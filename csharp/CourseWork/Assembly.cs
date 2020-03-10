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

            var lastLexeme = Lexemes.Last();
            if(lastLexeme.Tokens.Count != 1 ||
                lastLexeme.Tokens[0].Type != TokenType.EndKeyword)
            {
                return new Error(ErrorType.MissingEndKeyword);
            }

            return null;
        }

        public string ToSentenceTable()
        {
            var res = "";
            var i = 0;
            var flattenedTokens = Lexemes.SelectMany(x => x.Tokens).ToList();

            for (var l = 0; l < Lexemes.Count; l++)
            {
                var lexeme = Lexemes[l];
                res += string.Format("{0,-2} | {1,-40}\n", l, string.Join(" ", lexeme.Tokens.Select(p => $"{p.ToSourceValue(false)}({flattenedTokens.IndexOf(p)})")));
            }

            res += new string('=', 50) + "\n";
            res += string.Format("{0,-2} |{1,4} |{2,9} |{3,9} |{4,9} | {5,9}\n", 
                "#", "Lbl", "Mnemonic", "Op. 1", "Op. 2", "...");
            res += new string('=', 50) + "\n";

            for (var l = 0; l < Lexemes.Count; l++)
            {
                res += $"{l,-2} |";
                res += Lexemes[l].ToSentenceTableString(i);
                i += Lexemes[i].Tokens.Count;
            }

            return res;
        }
    }
}
