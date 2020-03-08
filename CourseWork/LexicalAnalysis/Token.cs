using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace CourseWork.LexicalAnalysis
{
    public class Token
    {
        public Lexeme ParrentLexeme { get; }

        public TokenType Type { get; internal set; }
        public string File { get; }
        public int Line { get; }
        public int CharIndex { get; }

        public string StringValue { get; }
    
        private string _shortName;


        private static readonly  Dictionary<string, TokenType> _dictionary = new Dictionary<string, TokenType>()
        {
            { "[",       TokenType.Symbol },
            { "]",       TokenType.Symbol },
            { ".",       TokenType.Symbol },
            { ",",       TokenType.Symbol },
            { "+",       TokenType.Symbol },
            { "-",       TokenType.Symbol },
            { ":",       TokenType.Symbol },

            { "eax",     TokenType.Register32 },
            { "ebx",     TokenType.Register32 },
            { "ecx",     TokenType.Register32 },
            { "edx",     TokenType.Register32 },

            { "ax",      TokenType.Register16 },
            { "bx",      TokenType.Register16 },
            { "cx",      TokenType.Register16 },
            { "dx",      TokenType.Register16 },
            { "sp",      TokenType.Register16 },
            { "bp",      TokenType.Register16 },
            { "si",      TokenType.Register16 },
            { "di",      TokenType.Register16 },

            { "al",      TokenType.Register8 },
            { "cl",      TokenType.Register8 },
            { "dl",      TokenType.Register8 },
            { "bl",      TokenType.Register8 },
            { "ah",      TokenType.Register8 },
            { "ch",      TokenType.Register8 },
            { "dh",      TokenType.Register8 },
            { "bh",      TokenType.Register8 },

            { "es",      TokenType.SegmentRegister },
            { "cs",      TokenType.SegmentRegister },
            { "ss",      TokenType.SegmentRegister },
            { "ds",      TokenType.SegmentRegister },
            { "fs",      TokenType.SegmentRegister },
            { "gs",      TokenType.SegmentRegister },

            { "db",      TokenType.DbDirective },
            { "dw",      TokenType.DwDirective },
            { "dd",      TokenType.DdDirective },
            { "equ",     TokenType.EquDirective },
            { "if",      TokenType.IfDirective },
            { "else",    TokenType.ElseDirective },
            { "endif",   TokenType.EndifDirective },

            { "movsb",   TokenType.Instruction },
            { "pop",     TokenType.Instruction },
            { "dec",     TokenType.Instruction },
            { "add",     TokenType.Instruction },
            { "cmp",     TokenType.Instruction },
            { "bt",      TokenType.Instruction },
            { "jbe",     TokenType.Instruction },
            { "mov",     TokenType.Instruction },
            { "or",      TokenType.Instruction },

            { "segment", TokenType.SegmentKeyword},
            { "end",     TokenType.EndKeyword},
            { "ends",    TokenType.EndsKeyword},
        };

        private static readonly Regex _numberHexRegex = new Regex(@"^[0-9a-f]+h$");
        private static readonly Regex _numberDecRegex = new Regex("^[0-9]+$");
        private static readonly Regex _numberBinRegex = new Regex("^[01]+b$");
        private static readonly Regex _identifierRegex = new Regex(@"^[a-z]\w*$");

        public Token(string stringValue, string file, int line, int charIndex, Lexeme lexeme, out Error error)
        {
            error = null;
            ParrentLexeme = lexeme;
            StringValue = stringValue;

            Line = line;
            CharIndex = charIndex;
            if (file != null && System.IO.File.Exists(file))
            {
                File = file;
                _shortName = new FileInfo(File).Name;
            }

            if (!_dictionary.TryGetValue(stringValue, out var type))
            {
                if (_numberHexRegex.IsMatch(stringValue))
                {
                    Type = TokenType.HexNumber;
                }
                else if (_numberDecRegex.IsMatch(stringValue))
                {
                    Type = TokenType.DecNumber;
                }
                else if (_numberBinRegex.IsMatch(stringValue))
                {
                    Type = TokenType.BinNumber;
                }
                else if (stringValue.Trim().StartsWith("\"") && stringValue.Trim().EndsWith("\""))
                {
                    Type = TokenType.Text;
                }
                else if(_identifierRegex.IsMatch(stringValue))
                {
                    Type = TokenType.Identifier;
                }
                else
                {
                    Type = TokenType.Unknown;
                    error = new Error(ErrorType.UnknownToken, this);
                    return;
                }
            }
            else
            {
                Type = type;
            }

            if(Type == TokenType.Text)
                StringValue = stringValue.Trim(' ', '\t', '\"');
        }

        public bool HasValue()
        {
            return
                Type == TokenType.Instruction ||
                Type == TokenType.Register32 ||
                Type == TokenType.Register16 ||
                Type == TokenType.Register8 ||
                Type == TokenType.Symbol ||
                Type == TokenType.Identifier ||
                Type == TokenType.HexNumber ||
                Type == TokenType.DecNumber ||
                Type == TokenType.BinNumber ||
                Type == TokenType.Text ||
                Type == TokenType.UserSegment ||
                Type == TokenType.SegmentRegister ||
                Type == TokenType.Label;
        }

        public bool IsNumber()
        {
            return
                Type == TokenType.HexNumber ||
                Type == TokenType.DecNumber ||
                Type == TokenType.BinNumber;
        }

        public int ToValue()
        {
            switch (Type)
            {
                case TokenType.HexNumber:
                    return Convert.ToInt32(StringValue.Substring(0, StringValue.Length - 1), 16);
                case TokenType.DecNumber:
                    return Convert.ToInt32(StringValue, 10);
                case TokenType.BinNumber:
                    return Convert.ToInt32(StringValue.Substring(0, StringValue.Length - 1), 2);
                default:
                    throw new NotSupportedException();
            }
        }

        public override string ToString()
        {
            var sb = new StringBuilder();
            sb.AppendFormat("{0} ", Type);
            if (HasValue())
            {
                sb.AppendFormat("({0}) ", StringValue);
                if (IsNumber())
                    sb.AppendFormat("({0}) ", ToValue());
            }

            if (File != null)
                sb.AppendFormat("in {0} at {1}:{2}", _shortName, Line, CharIndex);
            else
                sb.AppendFormat("at {0}:{1}", Line, CharIndex);

            return sb.ToString();
        }

        public string ToSourceValue(bool dedStyle)
        {
            if (dedStyle)
            {
                return $"({"\"" + StringValue + "\"", -5} : {Type} : {StringValue.Length})";
            }
            else
            {
                if (HasValue())
                {
                    return IsNumber() ? ToValue().ToString() : StringValue;
                }

                return Type.ToString();
            }
        }
    }
}