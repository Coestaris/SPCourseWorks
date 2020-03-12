package tokens

import (
	"course_work_2/types"
	"fmt"
)

type token struct {
	Value     string
	TokenType int
	Lexeme    types.Lexeme
	Line      int
	Char      int
}

func NewToken(value string, lexeme types.Lexeme, line, char int) (types.Token, error) {
	tokenType := typeByValue(value)
	if tokenType == -1 {
		return nil, fmt.Errorf("unknown token type passed:\n\tvalue: %s, line: %d, char: %d", value, line, char)
	}
	return &token{value, typeByValue(value), lexeme, line, char}, nil
}

func (t *token) GetTokenType() *int {
	return &t.TokenType
}

func (t *token) GetValue() string {
	return t.Value
}

func (t *token) ChangeType(tokenType int) {
	t.TokenType = tokenType
}

func (t *token) ToString() string {
	return fmt.Sprintf(`("%s" : %s : %d)`, t.Value, ToStringValue(t.TokenType), len(t.Value))
}