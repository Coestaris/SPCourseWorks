using CourseWork.LexicalAnalysis;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CourseWork
{
    public class Error
    {
        public Token Token { get; }

        public ErrorType ErrorType { get; }

        public Error(ErrorType error, Token token)
        {
            ErrorType = error;
            Token = token;
        }

        public Error(ErrorType error)
        {
            ErrorType = error;
        }

        public override string ToString()
        {
            if (Token != null)
                return $"Error: {ErrorType} near {Token.ToString()}";
            else
                return $"Error: {ErrorType}";
        }
    }
}
