package lexeme

import (
	"course_work_2/segment"
	ptokens "course_work_2/tokens"
	"course_work_2/types"
	"course_work_2/variable"
	"errors"
	"fmt"
	"strings"
)

const (
	Register8 = iota
	Register16
	Register32
	Constant8
	Constant16
	Constant32
	Mem8
	Mem16
	Mem32
	Ptr8
	Ptr16
	Ptr32
	LabelBackward
	LabelForward
)

type lexeme struct {
	program types.ASM
	tokens  []types.Token
	offset  int
	instruction types.Instruction
	segment types.Segment

	instructionIndex int
	labelIndex int
	hasName bool
	hasInstruction bool
	hasOperands bool
	operands []types.Operand
}

func Abs(x int64) int64 {
	if x < 0 {
		return -x
	} else {
		return x
	}
}

func (l *lexeme) PrettyPrint() (str string) {
	for _, t := range l.tokens {
		hasSpaceBefore := false
		hasSpaceAfter := false
		value := t.GetValue()

		if value == "," {
			hasSpaceAfter = true
		} else if value == "+" {
			hasSpaceAfter = true
			hasSpaceBefore = true
		} else if tType := t.GetTokenType(); tType == ptokens.INSTRUCTION || tType == ptokens.DIRECTIVE {
			hasSpaceAfter = true
		}

		if t.GetTokenType() == ptokens.DIRECTIVE && (value == "db" || value == "dd" || value == "dw") {
			hasSpaceBefore = true
		}

		if t.GetTokenType() == ptokens.DIRECTIVE || t.GetTokenType() == ptokens.END {
			value = strings.ToUpper(value)
		}

		if hasSpaceBefore {
			str += " "
		}

		str += value

		if hasSpaceAfter {
			str += " "
		}
	}

	ident := ""
	directive := l.GetInstructionToken()
	if val := directive.GetValue(); val == "db" || val == "dd" || val == "dw" {
		ident = "        "
	} else {
		if l.segment != nil {
			ident = "    "
		}
		if !l.hasName && directive.GetTokenType() != ptokens.END {
			ident += "    "
		}
	}

	return ident + str
}

func (l *lexeme) SetSegment(value types.Segment) {
	l.segment = value
}

func (l *lexeme) SetOffset(value int) {
	l.offset = value
}

func (l *lexeme) SetInstruction(value types.Instruction) {
	l.instruction = value
}

func (l *lexeme) GetOperands() []types.Operand {
	return l.operands
}

func NewLexeme(program types.ASM, tokens []types.Token) types.Lexeme {
	return &lexeme{program: program, tokens: tokens, labelIndex: -1, instructionIndex: 0, hasOperands: true,
		hasInstruction: false}
}

func (l *lexeme) HasOperands() bool {
	return l.hasOperands
}

func (l *lexeme) GetOffset() int {
	return l.offset
}

func (l *lexeme) HasInstructions() bool {
	return l.hasInstruction
}

func (l *lexeme) GetParentProgram() types.ASM {
	return l.program
}

func (l *lexeme) GetSegment() types.Segment {
	return l.segment
}

func (l *lexeme) GetTokens() []types.Token {
	return l.tokens
}

func (l *lexeme) SetTokens(tokens []types.Token) error {
	l.tokens = tokens

	switch tokenLen := len(tokens); {
	case tokenLen == 1 && tokens[0].GetTokenType() == ptokens.END:
		parent := l.GetParentProgram()
		if parent.GetCodeSegment() != nil &&
			parent.GetCodeSegment().GetOpen() != nil && parent.GetCodeSegment().GetClose() == nil {
			parent.GetCodeSegment().SetClose(tokens[0])
		}
		if parent.GetDataSegment() != nil &&
			parent.GetDataSegment().GetOpen() != nil && parent.GetDataSegment().GetClose() == nil {
			parent.GetDataSegment().SetClose(tokens[0])
		}
	// SEG START
	case tokenLen == 2 && tokens[0].GetValue() == ".":
		firstToken := tokens[0]
		switch tokens[1].GetTokenType() {
		case ptokens.DATA:
			firstToken.SetTokenType(ptokens.DATA)

			newSegment := segment.NewSegment()
			newSegment.SetOpen(tokens[1])
			parent := l.GetParentProgram()
			parent.SetDataSegment(newSegment)
			// if code segment was opened, close it at the same token
			if parent.GetCodeSegment() != nil &&
				parent.GetCodeSegment().GetOpen() != nil && parent.GetCodeSegment().GetClose() == nil {
				parent.GetCodeSegment().SetClose(firstToken)
			}
		case ptokens.CODE:
			firstToken.SetTokenType(ptokens.DATA)

			newSegment := segment.NewSegment()
			newSegment.SetOpen(tokens[1])
			parent := l.GetParentProgram()
			parent.SetCodeSegment(newSegment)
			// if data segment was opened, close it at the same token
			if parent.GetDataSegment() != nil &&
				parent.GetDataSegment().GetOpen() != nil && parent.GetDataSegment().GetClose() == nil {
				parent.GetDataSegment().SetClose(firstToken)
			}
		default:
			return errors.New("unknown segment type")
		}
	case tokenLen == 3 && tokens[0].GetTokenType() == ptokens.IDENTIFIER && tokens[1].GetTokenType() == ptokens.DIRECTIVE:
		// VARIABLE
		variables := l.program.GetVariables()
		variableFound := false
		for _, x := range variables {
			if x.GetName().GetValue() == tokens[0].GetValue() {
				variableFound = true
				break
			}
		}

		if variableFound {
			return errors.New("same variable already exists")
		}

		v := variable.NewVariable(tokens[1], tokens[0], tokens[2])
		/*_, err := scanType(v)
		if err != nil {
			return err
		}*/

		l.program.SetVariables(append(l.program.GetVariables(), v))
	case tokenLen >= 2 && tokens[0].GetTokenType() == ptokens.IDENTIFIER &&
			tokens[1].GetValue() == ":":
		// LABEL
		labels := l.program.GetLabels()
		labelFound := false
		for _, x := range labels {
			if x.GetValue() == tokens[0].GetValue() {
				labelFound = true
				break
			}
		}
		if labelFound {
			return errors.New("same label already exists")
		}

		l.tokens[0].SetTokenType(ptokens.LABEL)
		l.program.SetLabels(append(l.program.GetLabels(), l.tokens[0]))
	case tokenLen >= 3 && tokens[2].GetTokenType() == ptokens.PTR:
		// if there were multiple PTR types, this would be usable
		if tokens[1].GetTokenType() == ptokens.WORD {
			tokens[2].SetTokenType(ptokens.WORD)
		}
	}

	l.ParseOperands()
	return nil
}

func scanType(x types.Variable) (int, error) {
	switch x.GetDirective().GetValue() {
	case "db":
		if value := x.GetValue(); Abs(value.GetNumValue()) >= 0xFF {
			return 0, fmt.Errorf("incorrect num value (%d, %d)",
				value.GetLine(), value.GetChar())
		}
		return Mem8, nil
	case "dw":
		if value := x.GetValue(); Abs(value.GetNumValue()) >= 0xFFFF {
			return 0, fmt.Errorf("incorrect num value (%d, %d)",
				value.GetLine(), value.GetChar())
		}
		return Mem16, nil
	default:
		if value := x.GetValue(); Abs(value.GetNumValue()) >= 0xFFFFFFFF {
			return 0, fmt.Errorf("incorrect num value (%d, %d)",
				value.GetLine(), value.GetChar())
		}
		return Mem32, nil
	}
}

func (l *lexeme) GetOperandsInfo() error {
	if !l.hasInstruction || !l.hasOperands {
		return nil
	}

	for _, op := range l.operands {
		index := op.GetIndex()
		length := op.GetLength()
		op.SetOperandTokens(l.GetTokens()[index : index + length])
		switch opTokens := op.GetOperandTokens(); {
		case len(opTokens) == 1:
			token := opTokens[0]
			op.SetToken(token)

			switch token.GetTokenType() {
			case ptokens.REG8:
				op.SetOperandType(Register8)
			case ptokens.REG16:
				op.SetOperandType(Register16)
			case ptokens.REG32:
				op.SetOperandType(Register32)
			case ptokens.DEC:
				fallthrough
			case ptokens.BIN:
				fallthrough
			case ptokens.HEX:
				switch value := token.GetNumValue(); {
				case value < 2^8: op.SetOperandType(Constant8)
				case value < 2^16: op.SetOperandType(Constant16)
				default: op.SetOperandType(Constant32)
				}
			case ptokens.IDENTIFIER:
				for _, x := range l.program.GetVariables() {
					if x.GetName().GetValue() == token.GetValue() {
						opType, err := scanType(x)
						if err != nil {
							return err
						}
						op.SetOperandType(opType)
						break
					}
				}
				fallthrough
			case ptokens.LABEL:
				for _, x := range l.program.GetLabels() {
					if x.GetValue() == token.GetValue() {
						if x.GetLine() > token.GetLine() {
							op.SetOperandType(LabelForward)
						} else {
							op.SetOperandType(LabelBackward)
						}
					}
				}
			default:
				return fmt.Errorf("wrong token in operand %s", op.GetToken().GetValue())
			}
		case len(opTokens) == 3 && opTokens[1].GetTokenType() == ptokens.MODEL:
			continue
		case len(opTokens) == 3 && opTokens[0].GetTokenType() == ptokens.SEGREG:
			for _, x := range l.program.GetVariables() {
				if x.GetName().GetValue() == opTokens[2].GetValue() {
					opType, err := scanType(x)
					if err != nil {
						return err
					}
					op.SetOperandType(opType)
					break
				}
			}
			op.SetSegmentPrefix(opTokens[0])
			op.SetToken(opTokens[2])
		case len(opTokens) < 6:
			return fmt.Errorf("wrong token in operand %s", opTokens[0].GetValue())
		default:
			offset := 0
			for _, x := range l.program.GetVariables() {
				if x.GetName().GetValue() == opTokens[2].GetValue() {
					opType, err := scanType(x)
					if err != nil {
						return err
					}
					op.SetOperandType(opType)
					break
				}
			}
			switch opTokens[offset].GetTokenType() {
			case ptokens.WORD:
				offset++
				op.SetOperandType(Ptr16)
			case ptokens.DWORD:
				offset++
				op.SetOperandType(Ptr32)
			case ptokens.SEGREG:
				op.SetSegmentPrefix(opTokens[offset])
				offset += 2

				if len(opTokens) != 8 {
					return fmt.Errorf("wrong token in operand %s", opTokens[offset].GetValue())
				}
			}
			if opTokens[offset + 5].GetTokenType() == ptokens.SYM && opTokens[offset + 5].GetValue() == "*" {
				// scaled index
				/*if len(opTokens) != 8 {
					return fmt.Errorf("wrong token in operand %s", opTokens[offset].GetValue())
				}*/
				op.SetScaleToken(opTokens[offset + 6])
			}
			op.SetToken(opTokens[offset])
			op.SetSumFirstToken(opTokens[offset + 2])
			op.SetSumSecondToken(opTokens[offset + 4])
			break
		}
	}

	return nil
}

func (l *lexeme) GetInstruction() types.Instruction {
	return l.instruction
}

func (l *lexeme) GetInstructionToken() types.Token {
	return l.tokens[l.instructionIndex]
}

func (l *lexeme) GetName() types.Token {
	return l.tokens[0]
}

func (l *lexeme) appendInline() error{
	labels := l.program.GetLabels()
	for _, token := range l.tokens {
		labelFound := false
		for _, x := range labels {
			if x.GetValue() == token.GetValue() {
				labelFound = true
				break
			}
		}
		if labelFound {
			token.SetTokenType(ptokens.LABEL)
		}
	}

	l.tokens[0].SetTokenType(ptokens.LABEL)
	return nil
}

func (l *lexeme) hasLabel() bool {
	return len(l.tokens) >= 2 && l.tokens[0].GetTokenType() == ptokens.LABEL
}

func (l *lexeme) ToString() (str string) {
	i := 0
	for _, t := range l.tokens {
		str += fmt.Sprintf("%-30s", t.ToString())
		if i != 0 && i % 4 == 0 {
			str += "\n"
		}
		i += 1
	}

	return fmt.Sprintf("[%s]", strings.TrimSpace(str))
}

func (l *lexeme) ToSentenceTableString(level int) (res string) {
	if l.hasName {
		res += fmt.Sprintf("%5d |", level)
	} else {
		res += fmt.Sprintf("%5s |", "--")
	}

	if l.hasInstruction {
		res += fmt.Sprintf(" %2d |%2d |", level + l.instructionIndex, 1)
	}

	if l.hasOperands && l.hasInstruction {
		for j, op := range l.operands {
			s := ""
			if j == len(l.operands) {
				s = " |"
			}
			res += fmt.Sprintf(" %2d |%2d |%s", level + op.GetIndex(), op.GetLength(), s)
		}
	}

	return res
}

func (l *lexeme) ParseOperands() {
	offset := 0

	if l.hasLabel() {
		l.labelIndex = 0
		l.hasName = true
		offset += 2
	}

	if len(l.tokens) <= offset {
		l.hasInstruction = false
		return
	}

	if l.tokens[offset].GetTokenType() == ptokens.IDENTIFIER {
		l.hasName = true
		offset++
	}

	for _, x := range l.tokens {
		if tokenType := x.GetTokenType(); tokenType == ptokens.INSTRUCTION || tokenType == ptokens.DIRECTIVE {
			l.hasInstruction = true
			break
		}
	}

	if !l.hasInstruction {
		return
	}

	l.instructionIndex = offset
	offset++

	if len(l.tokens) <= offset {
		l.hasOperands = false
		return
	}

	/*if l.tokens[offset].GetTokenType() == ptokens.WORD {
		offset++
	}*/

	operandInd := 0
	l.operands = append(l.operands, NewOperand(offset, 0))

	for _, t := range l.tokens[offset:] {
		if t.GetTokenType() == ptokens.SYM && t.GetValue() == "," {
			l.operands = append(l.operands,
				NewOperand(l.operands[operandInd].GetLength() + l.operands[operandInd].GetIndex() + operandInd + 1,
					0))
			operandInd++
		} else {
			l.operands[operandInd].SetLength(l.operands[operandInd].GetLength() + 1)
		}
	}
	return
}