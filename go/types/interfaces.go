package types

type ASM interface {
	GetLexemes() *[]Lexeme
	GetLabels() *[]string
	GetSource() string

	Parse() error
	ToIndexedTable() string
	ToSentenceTable() string
}

type Lexeme interface {
	SetTokens(tokens []Token) error
	GetTokens() []Token
	GetParentProgram() ASM

	ToString() string
	ToSentenceTable() (int, int, []int, []int, bool, bool)
}

type Token interface {
	GetTokenType() *int
	GetValue() string

	ToDedStyle() string
}
