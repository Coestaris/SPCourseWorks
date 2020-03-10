package lexeme

import (
	ptokens "course_work_2/tokens"
	"course_work_2/types"
	"errors"
	"fmt"
	"strings"
)

type operand struct {
	index int
	length int
}

type lexeme struct {
	Program types.ASM
	Tokens  []types.Token

	instructionIndex int
	labelIndex int
	hasName bool
	hasInstruction bool
	hasOperands bool
	operands []*operand
}

func NewLexeme(program types.ASM, tokens []types.Token) types.Lexeme {
	return &lexeme{Program: program, Tokens: tokens}
}

func (l *lexeme) GetParentProgram() types.ASM {
	return l.Program
}

func (l *lexeme) GetTokens() []types.Token {
	return l.Tokens
}

func (l *lexeme) SetTokens(tokens []types.Token) error {
	l.Tokens = tokens

	if len(tokens) == 2 {
		if *tokens[0].GetTokenType() == ptokens.IDENTIFIER && *tokens[1].GetTokenType() == ptokens.SYM && tokens[1].GetValue() == ":" {
			labels := l.Program.GetLabels()
			labelFound := false
			for _, x := range *labels {
				if x == tokens[0].GetValue() {
					labelFound = true
					break
				}
			}
			if labelFound {
				return errors.New("same label already exists")
			}

			*l.Tokens[0].GetTokenType() = ptokens.LABEL
			*labels = append(*labels, l.Tokens[0].GetValue())
		}
	}

	return nil
}

func (l *lexeme) appendInline() error{
	labels := l.Program.GetLabels()
	for _, token := range l.Tokens {
		labelFound := false
		for _, x := range *labels {
			if x == token.GetValue() {
				labelFound = true
				break
			}
		}
		if labelFound {
			*token.GetTokenType() = ptokens.LABEL
		}
	}

	*l.Tokens[0].GetTokenType() = ptokens.LABEL
	return nil
}

func (l *lexeme) hasLabel() bool {
	return len(l.Tokens) >= 2 && *l.Tokens[0].GetTokenType() == ptokens.LABEL
}

func (l *lexeme) ToString() string {
	str := ""
	i := 0
	for _, t := range l.Tokens {
		str += fmt.Sprintf("%-30s", t.ToDedStyle())
		if i != 0 && i % 4 == 0 {
			str += "\n"
		}
		i += 1
	}

	return fmt.Sprintf("[%s]", strings.TrimSpace(str))
}

func (l *lexeme) ToSentenceTableString(level int) string {
	l.toSentenceTable()
	res := ""

	if l.hasName {
		res += fmt.Sprintf("%3d |", level)
	} else {
		res += fmt.Sprintf("%3s |", "--")
	}

	if l.hasInstruction {
		res += fmt.Sprintf("%3d %3d |", level + l.instructionIndex, 1)
	}

	if l.hasOperands && l.hasInstruction {
		for j, op := range l.operands {
			s := ""
			if j == len(l.operands) {
				s = " |"
			}
			res += fmt.Sprintf("%3d %3d%s", level + op.index, op.length, s)
		}
	}

	return res
}

func (l *lexeme) toSentenceTable() {
	l.labelIndex = -1
	l.instructionIndex = 0
	l.operands = []*operand{}
	l.hasOperands = true
	l.hasInstruction = true

	offset := 0

	if l.hasLabel() {
		l.labelIndex = 0
		offset += 2
	}

	if len(l.Tokens) <= offset {
		l.hasInstruction = false
		return
	}

	for _, x := range l.Tokens {
		if tokenType := *x.GetTokenType(); tokenType == ptokens.INSTRUCTION || tokenType == ptokens.DIRECTIVE {
			l.hasInstruction = false
		}
	}

	if !l.hasInstruction {
		return
	}

	if *l.Tokens[offset].GetTokenType() == ptokens.IDENTIFIER {
		offset++
	}

	l.instructionIndex = offset
	if !l.hasInstruction {
		offset++
	}

	if len(l.Tokens) <= offset {
		l.hasOperands = false
		return
	}

	operandInd := 0
	l.operands = append(l.operands, &operand{offset, 0})

	for _, t := range l.Tokens[offset:] {
		if *t.GetTokenType() == ptokens.SYM && t.GetValue() == "," {
			l.operands = append(l.operands, &operand{l.operands[operandInd].index, 0})
			operandInd++
		} else {
			l.operands[operandInd].length++
		}
	}
	return
}