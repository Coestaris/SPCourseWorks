package assembly

import (
	"course_work_2/parser"
	"course_work_2/segment"
	"course_work_2/tokens"
	"course_work_2/types"
	"fmt"
)

type asm struct {
	Source      string
	FileName    string
	Lexemes     []types.Lexeme
	Labels      []types.Token
	variables   []types.Variable
	dataSegment types.Segment
	codeSegment types.Segment
}

func (a *asm) SetLexemes(value []types.Lexeme) {
	a.Lexemes = value
}

func (a *asm) SetLabels(value []types.Token) {
	a.Labels = value
}

func (a *asm) SetVariables(value []types.Variable) {
	a.variables = value
}

func (a *asm) SetSource(value string) {
	a.Source = value
}

func (a *asm) SetDataSegment(value types.Segment) {
	a.dataSegment = value
}

func (a *asm) SetCodeSegment(value types.Segment) {
	a.codeSegment = value
}

func NewAssembly(source string, fileName string) types.ASM {
	return &asm{Source: source, FileName: fileName}
}

func (a *asm) GetSource() string {
	return a.Source
}

func (a *asm) GetLexemes() []types.Lexeme {
	return a.Lexemes
}

func (a *asm) GetLabels() []types.Token {
	return a.Labels
}

func (a *asm) GetVariables() []types.Variable {
	return a.variables
}

func (a *asm) GetDataSegment() types.Segment {
	return a.dataSegment
}

func (a *asm) GetCodeSegment() types.Segment {
	return a.codeSegment
}

func (a *asm) FirstPass() error {
	segment.ProceedSegments(a)
	for _, l := range a.GetLexemes() {
		if err := l.GetOperandsInfo(); err != nil {
			return err
		}
	}

	offset := 0
	for _, l := range a.GetLexemes() {
		if !l.HasInstructions() {
			l.SetOffset(-l.GetOffset())
			continue
		}

		directive := l.GetInstructionToken()
		switch directive.GetTokenType() {
		case tokens.CODE:
		case tokens.DATA:
		case tokens.END:
			l.SetOffset(-l.GetOffset())
			offset = 0
			continue
		case tokens.INSTRUCTION:
			inst := FindInfo(l)
			if inst == nil {
				return fmt.Errorf("wrong instruction format %s", l.GetTokens()[0].GetValue())
			}

			l.SetInstruction(inst)
			err := inst.CheckOpRestrictions(l)
			if err != nil {
				return err
			}
		}

		l.SetOffset(offset)
		size := GetSize(l)
		offset += size

		if l.GetSegment() != nil {
			l.GetSegment().SetSize(offset)
		}
	}
	return nil
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
