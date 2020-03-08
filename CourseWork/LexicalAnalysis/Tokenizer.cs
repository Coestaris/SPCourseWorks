using System;
using System.Collections.Generic;

namespace CourseWork.LexicalAnalysis
{
    internal class Tokenizer
    {
        public static List<char> tokenSplitCharacters = new List<char>()
        {
            '\t', ' ', '\n', '[', ']', '.', ',', '_','+', '-', ':',
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

        private static void TokensToLexeme(List<RawToken> rawTokens, Assembly assembly, out Error error)
        {
            error = null;
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
                    out error));
                if (error != null) return;
            }

            if (tokens.Count != 0)
            {
                lexeme.SetTokens(tokens, out error);
                if (error != null) return;

                assembly.Lexemes.Add(lexeme);
            }
            rawTokens.Clear();
        }

        public static void Tokenize(Assembly assembly, out Error error)
        {
            error = null;

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
                        currentToken = c.ToString();
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
                        currentToken += c;
                    }

                    if (c == '\n')
                    {
                        TokensToLexeme(rawTokens, assembly, out error);
                        if (error != null) return;

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
                error = new Error(ErrorType.UnclosedQuotes);
                return;
            }

            rawTokens.Add(new RawToken(currentToken, lineCount, charCount));

            TokensToLexeme(rawTokens, assembly, out error);
            if (error != null) return;
        }
    }
}