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

func splitter(s *string) *string {
	for i := 0; i < 50; i++ {
		*s += "="
	}
	*s += "\n"
	return s
}

func (a *asm) ToSentenceTable() string {
	res := ""
	i := 0

	splitter(&res)
	res += fmt.Sprintf(" %2s| %5s| %9s| %9s | %9s| ...\n", "#", "LABEL", "MENMONIC", "OPERAND 1", "OPERAND 2")
	splitter(&res)

	for li, l := range a.Lexemes {
		labInd, instInd, opIndexes, opLength, hasNoOperands, hasNoInstruction := l.ToSentenceTable()

		res += fmt.Sprintf("%-2d |", li)

		if labInd != -1 {
			res += fmt.Sprintf("%5d |", labInd + i)
		} else {
			res += fmt.Sprintf("%5s |", " none")
			labInd = 0
		}

		if !hasNoInstruction {
			res += fmt.Sprintf("%4d %4d |", i + instInd, 1)
		}

		if !hasNoInstruction && !hasNoOperands {
			for j := 0; j < len(opIndexes); j++ {
				s := ""
				if j != len(opIndexes) - 1 {
					s = " |"
				}

				res += fmt.Sprintf("%4d %4d%s", i + labInd + opIndexes[j], opLength[j], s)
			}
		}

		res += "\n"

		i += len(l.GetTokens())
	}
	splitter(&res)
	return res
}
