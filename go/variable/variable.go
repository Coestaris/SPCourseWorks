package variable

import "course_work_2/types"

type variable struct {
	directive types.Token
	name types.Token
	value types.Token
}

func (v variable) GetValue() types.Token {
	return v.value
}

func (v variable) GetName() types.Token {
	return v.name
}

func (v variable) GetDirective() types.Token {
	return v.directive
}

func NewVariable(directive, name, value types.Token) types.Variable {
	return &variable{directive, name, value}
}
