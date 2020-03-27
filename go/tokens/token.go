package tokens

import (
	"course_work_2/types"
	"fmt"
	"strconv"
)

type token struct {
	Value     string
	TokenType int
	Lexeme    types.Lexeme
	Line      int
	Char      int
}

func (t *token) GetLexeme() types.Lexeme {
	return t.Lexeme
}

func (t *token) SetTokenType(value int) {
	t.TokenType = value
}

func (t *token) SetValue(value string) {
	t.Value = value
}

func (t *token) SetLine(value int) {
	t.Line = value
}

func NewToken(value string, lexeme types.Lexeme, line, char int) (types.Token, error) {
	tokenType := typeByValue(value)
	if tokenType == -1 {
		return nil, fmt.Errorf("unknown token type passed:\n\tvalue: %s, line: %d, char: %d", value, line, char)
	}
	return &token{value, typeByValue(value), lexeme, line, char}, nil
}

func (t *token) GetTokenType() int {
	return t.TokenType
}

func (t *token) GetLine() int {
	return t.Line
}

func (t *token) GetValue() string {
	return t.Value
}

func (t *token) GetNumValue() int64 {
	switch t.TokenType {
	case HEX:
		num, err := strconv.ParseInt(t.Value[:len(t.Value)-1], 16, 64)
		if err != nil {
			panic(err)
		}
		return num
	case DEC:
		num, err := strconv.Atoi(t.Value)
		if err != nil {
			panic(err)
		}
		return int64(num)
	default:
		num, err := strconv.ParseInt(t.Value[:len(t.Value)-1], 2, 64)
		if err != nil {
			panic(err)
		}
		return num
	}
}

func (t *token) ChangeType(tokenType int) {
	t.TokenType = tokenType
}

func (t *token) ToString() string {
	sType := ToStringValue(t.TokenType)
	if t.TokenType == LABEL {
		sType = ToStringValue(IDENTIFIER)
	}
	return fmt.Sprintf(`("%9s" : %11s : %d)`, t.Value, sType, len(t.Value))
}