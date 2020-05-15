package parser

import (
	"course_work_2/lexeme"
	tokens2 "course_work_2/tokens"
	"course_work_2/types"
	"strings"
)

const splitChars = "\t \n[].,+-:*"

type rawToken struct {
	value string
	line  int
	char  int
}

func getTypedTokens(tokens []rawToken, program types.ASM) (types.Lexeme, error) {
	lex := lexeme.NewLexeme(program, []types.Token{})

	var clearedTokens []types.Token
	for _, rawToken := range tokens {
		if strings.TrimSpace(rawToken.value) != "" {
			t, err := tokens2.NewToken(rawToken.value, lex, rawToken.line, rawToken.char)
			if err != nil {
				return nil, err
			}
			clearedTokens = append(clearedTokens, t)
		}
	}

	err := lex.SetTokens(clearedTokens)
	if err != nil {
		return nil, err
	}

	return lex, nil
}

func Parse(program types.ASM) (lexemes []types.Lexeme, errors []error) {
	token := ""

	line := 1
	char := 1
	var tokens []rawToken
	lastEl := false

	for _, c := range program.GetSource() {
		if strings.ContainsRune(splitChars, c) {
			tokens = append(tokens, rawToken{token, line, char})
			token = strings.ToLower(string(c))
			lastEl = true
		} else {
			if lastEl {
				tokens = append(tokens, rawToken{token, line, char})
				token = ""
			}
			lastEl = false
			token += strings.ToLower(string(c))
		}

		if c == '\n' {
			t, err := getTypedTokens(tokens, program)
			if err != nil {
				errors = append(errors, err)
			} else if len(t.GetTokens()) != 0 {
				lexemes = append(lexemes, t)
			}

			tokens = []rawToken{}
			line++
			char = 1
		} else {
			char++
		}
	}

	tokens = append(tokens, rawToken{token, line, char})

	t, err := getTypedTokens(tokens, program)
	if err != nil {
		errors = append(errors, err)
		return nil, errors
	}

	if len(t.GetTokens()) != 0 {
		lexemes = append(lexemes, t)
	}
	return lexemes, errors
}
