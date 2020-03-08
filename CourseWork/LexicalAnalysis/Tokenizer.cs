using System;
using System.Collections.Generic;

namespace CourseWork.LexicalAnalysis
{
    public class Tokenizer
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

        public static List<Lexeme> Tokenize(string input)
        {
            return Tokenize(input, null);
        }

        public static List<Lexeme> Tokenize(string input, string fileName)
        {
            var rawTokens = new List<RawToken>();
            var currentToken = "";
            var lastContains = false;
            var lineCount = 0;
            var charCount = 0;
            var listeningString = false;

            var lexemes = new List<Lexeme>();

            foreach (var c in input)
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

                        rawTokens.ForEach(p => p.Token = p.Token.Trim());
                        rawTokens = rawTokens.FindAll(p => !string.IsNullOrWhiteSpace(p.Token));

                        if (rawTokens.Count != 0)
                        {
                            var tokens = new List<Token>();
                            foreach (var token in rawTokens)
                            {
                                tokens.Add(new Token(token.Token, fileName, token.LineIndex, token.charIndex));
                            }

                            lexemes.Add(new Lexeme(tokens));
                        }

                        rawTokens.Clear();


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

            if (lastContains)
                rawTokens.Add(new RawToken(currentToken, lineCount, charCount));

            rawTokens.ForEach(p => p.Token = p.Token.Trim());
            rawTokens = rawTokens.FindAll(p => !string.IsNullOrWhiteSpace(p.Token));

            if (rawTokens.Count != 0)
            {
                var _tokens = new List<Token>();
                foreach (var token in rawTokens)
                {
                    _tokens.Add(new Token(token.Token, fileName, token.LineIndex, token.charIndex));
                }

                lexemes.Add(new Lexeme(_tokens));
            }

            return lexemes;
        }
    }
}