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
        public List<Variable> UserVariables { get; }

        public Dictionary<string, Token> Equs{ get; }

        public Assembly(AssemblySource source)
        {
            Lexemes = new List<Lexeme>();
            UserSegments = new List<UserSegment>();
            UserLabels = new List<Token>();
            Equs = new Dictionary<string, Token>();
            UserVariables = new List<Variable>();

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

        private Error ProceedConditionalDirectives()
        {
            var toDelete = new List<int>();
            var openedIf = false;
            var openedElse = false;
            var ifCond = false;

            for(var i = 0; i < Lexemes.Count; i++)
            {
                var lexeme = Lexemes[i];
                if (lexeme.Tokens[0].Type == TokenType.IfDirective)
                {
                    if(openedIf || openedElse)
                        return new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);

                    openedIf = true;

                    if (lexeme.Tokens.Count != 2)
                        return new Error(ErrorType.WrongDirectiveFormat, lexeme.Tokens[0]);

                    var cond = lexeme.Tokens[1];
                    if (cond.IsNumber())
                        ifCond = cond.ToValue() != 0;

                    else return new Error(ErrorType.ConstantExpected, lexeme.Tokens[0]);

                    toDelete.Add(i);
                }
                else if (lexeme.Tokens[0].Type == TokenType.ElseDirective)
                {
                    if(!openedIf)
                        return new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);

                    openedIf = false;
                    openedElse = true;

                    toDelete.Add(i);
                }
                else if (lexeme.Tokens[0].Type == TokenType.EndifDirective)
                {
                    if(!openedIf && !openedElse)
                        return new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);

                    openedElse = false;
                    openedIf = false;

                    toDelete.Add(i);
                }
                else
                {
                    if(openedIf && !ifCond)
                        toDelete.Add(i);
                    else if (openedElse && ifCond)
                        toDelete.Add(i);
                }
            }

            toDelete.Reverse();
            foreach (var index in toDelete)
                Lexemes.RemoveAt(index);

            return null;
        }

        private Error ProceedSegmentCounter()
        {
            UserSegment segment = null;
            foreach (var lexeme in Lexemes)
            {
                if (lexeme.Tokens.Count == 2 &&
                    lexeme.Tokens[1].Type == TokenType.SegmentKeyword)
                {
                    segment = UserSegments.Find(p => p.Name == lexeme.Tokens[0].StringValue);
                }
                else if (lexeme.Tokens.Count == 2 && lexeme.Tokens[1].Type == TokenType.EndsKeyword)
                {
                    segment = null;
                }
                else
                {
                    // Только Equ может не иметь сегмента
                    if(segment == null)
                        if (lexeme.Tokens.Count == 3 && lexeme.Tokens[1].Type == TokenType.EquDirective ||
                            lexeme.Tokens.Count == 1 && lexeme.Tokens[0].Type == TokenType.EndKeyword)
                            continue;
                        else
                            return new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);

                    lexeme.Segment = segment;
                }
            }
            return null;
        }

        private Error AssignInfos()
        {
            var offset = 0;
            foreach (var lexeme in Lexemes)
            {
                if (lexeme.Structure.HasInstruction)
                {
                    var instruction = lexeme.Tokens[lexeme.Structure.InstructionIndex];
                    if (instruction.Type == TokenType.Instruction)
                    {

                        if (!InstructionInfo.Match(lexeme, out var info))
                            return new Error(ErrorType.InstructionFormat, lexeme.Tokens[0]);
                        lexeme.InstructionInfo = info;
                    }
                }

                lexeme.Offset = offset;
                offset += lexeme.GetSize();

                // segment declaration or closing
                if (lexeme.Segment == null)
                    offset = 0;
            }

            return null;
        }

        public Error FirstPass()
        {
            Error error;

            // Replace EQUs
            foreach (var lexeme in Lexemes)
                lexeme.ProceedEqu();

            // Proceed if else endif
            if ((error = ProceedConditionalDirectives()) != null)
                return error;

            // Proceed segment counter
            if ((error = ProceedSegmentCounter()) != null)
                return error;

            // Get all information about instruction operands
            foreach(var lexeme in Lexemes)
                if((error = lexeme.Structure.AnalyzeOperands()) != null)
                    return error;

            // Assign instruction infos to lexemes
            if ((error = AssignInfos()) != null)
                return error;

            return null;
        }
    }
}
