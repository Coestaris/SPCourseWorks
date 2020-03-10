package lexeme

import (
	ptokens "course_work_2/tokens"
	"course_work_2/types"
	"errors"
	"fmt"
	"strings"
)

type lexeme struct {
	Program types.ASM
	Tokens  []types.Token
}

func NewLexeme(program types.ASM, tokens []types.Token) types.Lexeme {
	return &lexeme{program, tokens}
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
		str += fmt.Sprintf("%30s", t.ToDedStyle())
		if i != 0 && i % 4 == 0 {
			str += "\n"
		}
		i += 1
	}

	return fmt.Sprintf("[%s]", strings.TrimSpace(str))
}

func (l *lexeme) ToSentenceTable() (int, int, []int, []int, bool, bool){
	labInd := -1
	instInd := 0
	var opIndexes []int
	var opLength []int
	hasNoOperands := false
	hasNoInstruction := false

	offset := 0

	if l.hasLabel() {
		labInd = 0
		offset += 2
	}

	if len(l.Tokens) <= offset {
		hasNoInstruction = true
		return labInd, instInd, opIndexes, opLength, false, hasNoInstruction
	}

	for _, x := range l.Tokens {
		if tokenType := *x.GetTokenType(); tokenType == ptokens.INSTRUCTION || tokenType == ptokens.DIRECTIVE {
			hasNoInstruction = true
		}
	}

	if hasNoInstruction {
		return labInd, instInd, opIndexes, opLength, false, hasNoInstruction
	}

	if *l.Tokens[offset].GetTokenType() == ptokens.IDENTIFIER {
		offset++
	}

	instInd = offset
	if !hasNoInstruction {
		offset++
	}

	if len(l.Tokens) <= offset {
		hasNoOperands = true
		return labInd, instInd, opIndexes, opLength, hasNoOperands, hasNoInstruction
	}

	operand := 0
	opIndexes = append(opIndexes, offset)
	opLength = append(opLength, 0)

	for _, t := range l.Tokens[offset:] {
		if *t.GetTokenType() == ptokens.SYM && t.GetValue() == "," {
			opIndexes = append(opIndexes, opLength[operand])
			opLength = append(opLength, 0)
			operand++
		} else {
			opLength[operand]++
		}
	}
	return labInd, instInd, opIndexes, opLength, hasNoOperands, hasNoInstruction
}