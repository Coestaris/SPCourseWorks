using CourseWork;
using CourseWork.DataStructures;
using CourseWork.LexicalAnalysis;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using kurs.DataStructures;

namespace CourseWork
{
    public class Assembly
    {
        public AssemblySource Source { get; }
        public List<Lexeme> Lexemes { get; }
        public List<UserSegment> UserSegments { get; }
        public List<Token> UserLabels { get; }
        public List<Variable> UserVariables { get; }
        public List<Equ> UserEqus{ get; }

        public Assembly(AssemblySource source)
        {
            Lexemes = new List<Lexeme>();
            UserSegments = new List<UserSegment>();
            UserLabels = new List<Token>();
            UserEqus = new List<Equ>();
            UserVariables = new List<Variable>();

            Source = source;
        }

        public void Parse()
        {
            Tokenizer.Tokenize(this);

            foreach(var us in UserSegments)
            {
                if (!us.Closed)
                    us.OpenToken.ParentLexeme.Error = new Error(ErrorType.UnclosedSegment, us.OpenToken);
                if (!us.Opened)
                    us.CloseToken.ParentLexeme.Error = new Error(ErrorType.UnopenedSegment, us.CloseToken);
            }

            // Replace EQUs
            foreach (var lexeme in Lexemes)
            {
                if (lexeme.Error == null)
                {
                    lexeme.ProceedEqu();
                    lexeme.Structure = lexeme.GetStructure();
                }
            }

            foreach (var lexeme in Lexemes)
            {
                if (lexeme.Error == null)
                {
                    if (!lexeme.HasLabel && lexeme.Structure.HasName && lexeme.Structure != null &&
                        lexeme.Structure.HasInstruction && lexeme.Tokens[lexeme.Structure.InstructionIndex].Type ==
                        TokenType.Instruction)
                    {
                        lexeme.Error = new Error(ErrorType.UnexpectedToken, lexeme.Tokens[0]);
                        continue;
                    }

                    if (!lexeme.HasLabel && lexeme.Structure != null && lexeme.Structure.HasName &&
                        !lexeme.Structure.HasInstruction)
                    {
                        lexeme.Error = new Error(ErrorType.UnexpectedToken, lexeme.Tokens[0]);
                        continue;
                    }

                    lexeme.AssignInlineUserSegmentsAndLabels(out var error);
                    if(error != null)
                        lexeme.Error = error;
                }
            }

            var lastLexeme = Lexemes.Last();
            if(lastLexeme.Tokens.Count != 1 ||
                lastLexeme.Tokens[0].Type != TokenType.EndKeyword)
            {
                lastLexeme.Error = new Error(ErrorType.MissingEndKeyword);
            }
        }

        private void ProceedConditionalDirectives()
        {
            var toDelete = new List<int>();
            var openedIf = false;
            var openedElse = false;
            var ifCond = false;

            for (var i = 0; i < Lexemes.Count; i++)
            {
                var lexeme = Lexemes[i];
                if (lexeme.Error != null) continue;

                if (lexeme.Tokens[0].Type == TokenType.IfDirective)
                {
                    if (openedIf || openedElse)
                    {
                        lexeme.Error = new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);
                        continue;
                    }

                    openedIf = true;

                    if (lexeme.Tokens.Count != 2)
                    {
                        lexeme.Error = new Error(ErrorType.WrongDirectiveFormat, lexeme.Tokens[0]);
                        continue;
                    }

                    var cond = lexeme.Tokens[1];
                    if (cond.IsNumber())
                        ifCond = cond.ToValue() != 0;

                    else
                    {
                        lexeme.Error = new Error(ErrorType.IncorrectInstructionTypes, lexeme.Tokens[0]);
                        continue;
                    }

                    toDelete.Add(i);
                }
                else if (lexeme.Tokens[0].Type == TokenType.ElseDirective)
                {
                    if (!openedIf)
                    {
                        lexeme.Error = new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);
                        continue;
                    }

                    openedIf = false;
                    openedElse = true;

                    toDelete.Add(i);
                }
                else if (lexeme.Tokens[0].Type == TokenType.EndifDirective)
                {
                    if (!openedIf && !openedElse)
                    {
                        lexeme.Error = new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);
                        continue;
                    }

                    openedElse = false;
                    openedIf = false;

                    toDelete.Add(i);
                }
                else
                {
                    if (openedIf && !ifCond)
                        toDelete.Add(i);
                    else if (openedElse && ifCond)
                        toDelete.Add(i);
                }
            }

            if (openedIf || openedElse)
            {
                var lastLexeme = Lexemes.Last();
                lastLexeme.Error = new Error(ErrorType.MissingEndKeyword);
            }

            toDelete.Reverse();
            foreach (var index in toDelete)
                Lexemes.RemoveAt(index);
        }

        private void ProceedSegmentCounter()
        {
            UserSegment segment = null;
            foreach (var lexeme in Lexemes)
            {
                if(lexeme.Error != null) continue;

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
                        {
                            lexeme.Error = new Error(ErrorType.UnexpectedDirective, lexeme.Tokens[0]);
                            continue;
                        }

                    lexeme.Segment = segment;
                }
            }
        }

        private void AssignInfos()
        {
            var offset = 0;
            foreach (var lexeme in Lexemes)
            {
                if(lexeme.Error != null)
                    continue;

                if (lexeme.Structure.HasInstruction)
                {
                    var instruction = lexeme.Tokens[lexeme.Structure.InstructionIndex];
                    if (instruction.Type == TokenType.Instruction)
                    {
                        int error = 0;
                        if ((error = InstructionInfo.Match(lexeme, out var info)) != 0)
                        {
                            lexeme.Error = new Error(
                                error == 1 ?
                                    ErrorType.UnknownInstruction :
                                    ErrorType.IncorrectInstructionTypes,
                                lexeme.Tokens[0]);
                            continue;
                        }

                        lexeme.InstructionInfo = info;
                    }
                }

                lexeme.Offset = offset;
                lexeme.Size = lexeme.GetSize();
                offset += lexeme.Size;

                if (lexeme.Segment != null)
                    lexeme.Segment.Size += lexeme.Size;

                // segment declaration or closing
                if (lexeme.Segment == null)
                    offset = 0;
            }
        }

        public void FirstPass()
        {
            Error error;

            // Proceed if else endif
            ProceedConditionalDirectives();

            // Proceed segment counter
            ProceedSegmentCounter();

            // Get all information about instruction operands
            foreach (var lexeme in Lexemes)
                if (lexeme.Error == null)
                {
                    if ((error = lexeme.Structure.AnalyzeOperands()) != null)
                        lexeme.Error = error;
                }

            // Assign instruction infos to lexemes
            AssignInfos();
        }

        public void SecondPass()
        {
            foreach (var lexeme in Lexemes)
                if (lexeme.Error == null)
                {
                    lexeme.GetBytes();
                }
        }
    }
}
