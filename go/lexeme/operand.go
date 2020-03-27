package lexeme

import "course_work_2/types"

type operand struct {
	operandType int
	index int
	length int
	operandTokens []types.Token
	token types.Token
	segmentPrefix types.Token
	sumTk1 types.Token
	sumTk2 types.Token
	scaleToken types.Token
}

func (o *operand) GetScaleToken() types.Token {
	return o.scaleToken
}

func (o *operand) SetScaleToken(value types.Token)  {
	o.scaleToken = value
}

func (o *operand) GetSumFirstToken() types.Token {
	return o.sumTk1
}

func (o *operand) GetSumSecondToken() types.Token {
	return o.sumTk2
}

func (o *operand) SetSumFirstToken(value types.Token) {
	o.sumTk1 = value
}

func (o *operand) SetSumSecondToken(value types.Token) {
	o.sumTk2 = value
}

func (o *operand) GetSegmentPrefix() types.Token {
	return o.segmentPrefix
}

func (o *operand) SetSegmentPrefix(value types.Token) {
	o.segmentPrefix = value
}

func (o *operand) SetOperandTokens(value []types.Token) {
	o.operandTokens = value
}

func (o *operand) SetIndex(value int) {
	o.index = value
}

func (o *operand) SetLength(value int) {
	o.length = value
}

func (o *operand) SetToken(value types.Token) {
	o.token = value
}

func (o *operand) SetOperandType(value int) {
	o.operandType = value
}

func (o *operand) GetOperandType() int {
	return o.operandType
}

func (o *operand) GetIndex() int {
	return o.index
}

func (o *operand) GetLength() int {
	return o.length
}

func (o *operand) GetToken() types.Token {
	return o.token
}

func NewOperand(index, length int) types.Operand {
	return &operand{index: index, length: length}
}

func (o *operand) GetOperandTokens() []types.Token {
	return o.operandTokens
}
