package assembly

import (
	"course_work_2/parser"
	"course_work_2/types"
	"fmt"
)

type asm struct {
	Source string
	FileName string
	Lexemes []types.Lexeme
	Labels []string
}

func NewAssembly(source string, fileName string) types.ASM {
	return &asm{source, fileName, []types.Lexeme{}, []string{}}
}

func (a *asm) GetSource() string {
	return a.Source
}

func (a *asm) GetLexemes() *[]types.Lexeme {
	return &a.Lexemes
}

func (a *asm) GetLabels() *[]string {
	return &a.Labels
}

func (a *asm) Parse() error {
	var err error
	a.Lexemes, err = parser.Parse(a)
	return err
}

func (a *asm) ToIndexedTable() string {
	res := ""
	i := 0
	for li, l := range a.Lexemes {
		res += fmt.Sprintf("%2d | ", li)
		for ti, t := range l.GetTokens() {
			res += fmt.Sprintf("%d(%s)", ti + i, t.GetValue())
		}
		res += "\n"
		i += len(l.GetTokens())
	}

	return res
}
