package types

type ASM interface {
	GetLexemes() *[]Lexeme
	GetLabels() *[]string
	GetSource() string

	ToIndexedTable() string

	Parse() error
}

type Lexeme interface {
	SetTokens(tokens []Token) error
	GetTokens() []Token
	GetParentProgram() ASM

	ToString() string
 	ToSentenceTableString(level int) string
}

type Token interface {
	GetTokenType() *int
	GetValue() string

	ToString() string
}
