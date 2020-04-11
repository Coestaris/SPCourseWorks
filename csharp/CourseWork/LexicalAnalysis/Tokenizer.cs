using System;
using System.Collections.Generic;
using System.Linq;

namespace CourseWork.LexicalAnalysis
{
    internal class Tokenizer
    {
        private static readonly List<char> tokenSplitCharacters = new List<char>
        {
            '\t', ' ', '\n', '[', ']', '.', ',', '_', '+', ':',
        };

        private struct RawToken
        {
            public string Token;
            public int LineIndex;
            public int charIndex;

            public RawToken(string token, int lineIndex, int charIndex)
            {
                Token = token;
                LineIndex = lineIndex;
                this.charIndex = charIndex;
            }
        }

        private static void TokensToLexeme(List<RawToken> rawTokens, Assembly assembly)
        {
            rawTokens.ForEach(p => p.Token = p.Token.Trim());

            var tokens = new List<Token>();
            var lexeme = new Lexeme(assembly);
            foreach (var token in rawTokens.FindAll(p => !string.IsNullOrWhiteSpace(p.Token)))
            {
                tokens.Add(new Token(
                    token.Token, 
                    assembly.Source.FileName, 
                    token.LineIndex, token.charIndex,
                    lexeme,
                    out var error));
                if(error != null)
                    lexeme.Error = error;
            }

            if (tokens.Count != 0)
            {
                lexeme.InsertTokens(tokens, out var error);
                if(error != null)
                    lexeme.Error = error;
                assembly.Lexemes.Add(lexeme);
            }
            rawTokens.Clear();
        }

        public static void Tokenize(Assembly assembly)
        {
            var rawTokens = new List<RawToken>();
            var currentToken = "";
            var lastContains = false;
            var lineCount = 0;
            var charCount = 0;
            var listeningString = false;

            foreach (var c in assembly.Source.Source)
            {
                if (c == '\"')
                {
                    if (listeningString)
                    {
                        listeningString = false;
                        currentToken += "\"";
                        rawTokens.Add(new RawToken(currentToken, lineCount, charCount));
                        currentToken = "";
                        continue;
                    }
                    else
                    {
                        listeningString = true;
                    }
                }

                if (!listeningString)
                {
                    if (tokenSplitCharacters.Contains(c))
                    {
                        rawTokens.Add(new RawToken(currentToken, lineCount, charCount));
                        currentToken = char.ToLower(c).ToString();
                        lastContains = true;
                    }
                    else
                    {
                        if (lastContains)
                        {
                            rawTokens.Add(new RawToken(currentToken, lineCount, charCount));
                            currentToken = "";
                        }

                        lastContains = false;
                        currentToken += char.ToLower(c);
                    }

                    if (c == '\n')
                    {
                        TokensToLexeme(rawTokens, assembly);

                        lineCount++;
                        charCount = 0;
                    }
                }
                else
                    currentToken += c.ToString();

                if (c != '\n')
                {
                    charCount++;
                }
            }

            if(listeningString)
            {
                assembly.Lexemes.Last().Error =
                    new Error(ErrorType.UnclosedQuotes, assembly.Lexemes.Last().Tokens.Last());
                return;
            }

            rawTokens.Add(new RawToken(currentToken, lineCount, charCount));

            TokensToLexeme(rawTokens, assembly);
        }
    }
}