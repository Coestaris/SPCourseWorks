package variable

import "course_work_2/types"

type variable struct {
	directive types.Token
	name types.Token
	value types.Token
}

func NewVariable(directive, name, value types.Token) types.Variable {
	return &variable{directive, name, value}
}