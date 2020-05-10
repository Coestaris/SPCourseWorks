package types

type ASM interface {
	GetLexemes() []Lexeme
	GetLabels() []Token
	GetVariables() []Variable
	GetSource() string
	GetDataSegment() Segment
	GetCodeSegment() Segment

	SetLexemes([]Lexeme)
	SetLabels([]Token)
	SetVariables([]Variable)
	SetSource(string)
	SetDataSegment(Segment)
	SetCodeSegment(Segment)

	ToIndexedTable() string

	Parse() []error
	FirstPass() []error
}

type Operand interface {
	GetOperandTokens() []Token
	GetIndex() int
	GetLength() int
	GetToken() Token
	GetOperandType() int
	GetSegmentPrefix() Token
	GetSumFirstToken() Token
	GetSumSecondToken() Token
	GetScaleToken() Token

	SetOperandTokens([]Token)
	SetIndex(int)
	SetLength(int)
	SetToken(Token)
	SetOperandType(int)
	SetSegmentPrefix(Token)
	SetSumFirstToken(Token)
	SetSumSecondToken(Token)
	SetScaleToken(Token)
}

type Lexeme interface {
	SetTokens(tokens []Token) error
	GetTokens() []Token
	GetParentProgram() ASM
	GetOperandsInfo() error
	GetSegment() Segment
	GetInstruction() Instruction
	GetInstructionToken() Token
	GetOffset() int
	GetOperands() []Operand
	HasInstructions() bool
	HasOperands() bool

	SetOffset(int)
	SetInstruction(Instruction)
	SetSegment(Segment)
	ToString() string
	ParseOperands()
	ToSentenceTableString(level int) string

	PrettyPrint() string
}

type Token interface {
	GetTokenType() int
	GetValue() string
	GetNumValue() int64
	GetLine() int
	GetLexeme() Lexeme

	SetTokenType(int)
	SetValue(string)
	SetLine(int)

	ToString() string
}

type Segment interface {
	GetOpen() Token
	GetClose() Token
	GetSize() int

	SetOpen(Token)
	SetClose(Token)
	SetSize(int)
}

type Instruction interface {
	GetName() string
	GetOpTypes() []int

	SetName(string)
	SetOpTypes([]int)

	CheckOpRestrictions(l Lexeme) error
}

type Variable interface {
}
