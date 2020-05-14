package assembly

import (
	"course_work_2/lexeme"
	"course_work_2/types"
	"fmt"
)

// ModRM
const (
	ModReg           = 0b11
	ModMemPlusDisp8  = 0b01
	ModMemPlusDisp32 = 0b10
	RMSib            = 0b100
	RMDisp           = 0b101

)

// Prefix bytes
const (
	expansionCode = 0x0F
	use16         = 0x66
)

// Segment Prefixes
var segPrefixes = map[string]byte {
	"es": 0x26,
	"cs": 0x2E,
	"ss": 0x36,
	"ds": 0x3E,
	"fs": 0x64,
	"gs": 0x65,
}

var regCodes = map[string]byte {
	"al": 0b000,
	"cl": 0b001,
	"dl": 0b010,
	"bl": 0b011,
	"ah": 0b100,
	"ch": 0b101,
	"dh": 0b110,
	"bh": 0b111,

	"ax": 0b000,
	"cx": 0b001,
	"dx": 0b010,
	"bx": 0b011,
	"sp": 0b100,
	"bp": 0b101,
	"si": 0b110,
	"di": 0b111,

	"eax": 0b000,
	"ecx": 0b001,
	"edx": 0b010,
	"ebx": 0b011,
	"esp": 0b100,
	"ebp": 0b101,
	"esi": 0b110,
	"edi": 0b111,
}

type instruction struct {
	// x86 Instruction Format

	// 1-Byte but in reality may be 2 with an expansion prefix
	// _________________________________
	// | - | - | - | - | - | - |d/x| s |
	// ---------------------------------
	// d: destination bit, 0 - add REG to R/M, 1 - add R/M to REG
	// x: used for immediate operands, indicates if constant is the same size as s
	// s: 0 - 8-bit operands, 1 - 32-bit operands
	opCode                byte

	// MOD-R/M section
	modSet                bool
	modRMIndex            byte
	modRM                 byte

	regOpCodeExtensionSet bool
	regOpCodeExtension    byte

	// SIB section
	sibSet                bool
	sib                   byte

	// packedRegister means that register info is packed inside last 3 bits of opcode
	packedRegister        bool
	packedSize            bool

	// Displacement
	disp                  byte

	// Immediate value
	immSet                bool
	immSize               byte
	imm                   []byte

	// Optional instruction prefixes
	expansionPrefix       bool
	addressSizePrefix     bool

	// Set this to true if we work with 16bit operand, if not = 8/32 bit
	operandSizePrefix     bool
	segmentOverridePrefix bool

	source                byte
	dest                  byte

	name                  string
	opTypes               []int
	opRestrictions        []string
}

func Int32ToBytes(v int) []byte {
	return []byte{byte(v >> 24) & 0xFF, byte(v >> 16) & 0xFF, byte(v >> 8) & 0xFF, byte(v) & 0xFF}
}

func NewSib(ss, index, base byte) byte {
	return ss << 6 | index << 3 | base
}

func NewModRM(mod, reg, rm byte) byte {
	return mod << 6 | reg << 3 | rm
}

func (i *instruction) PackReg(packedReg types.Token) error {
	if i.packedRegister {
		regNotFound := true
		for reg, val := range regCodes {
			if packedReg.GetValue() == reg {
				regNotFound = false
				i.opCode |= val
				break
			}
		}

		if regNotFound {
			return fmt.Errorf("unkown register %s", packedReg.GetValue())
		}
		return nil
	}
	return fmt.Errorf("instruction not marked as having packed opCode")
}

func (i *instruction) GetName() string {
	return i.name
}

func GetBytes(l types.Lexeme) (bytes []byte, err error) {
	inst := l.GetInstruction()

	if inst != nil {
		i := inst.(*instruction)

		// #1 Prefix Bytes | [0-4] bytes
		if i.operandSizePrefix {
			bytes = append(bytes, use16)
		}

		// #2 Expansion Prefix | [0-1] bytes
		if i.expansionPrefix {
			bytes = append(bytes, expansionCode)
		}

		// #3 Segment Prefix | [0-1] bytes
		for _, op := range l.GetOperands() {
			if seg := op.GetSegmentPrefix(); seg != nil {
				if _, ok := segPrefixes[seg.GetValue()]; !ok {
					return nil, fmt.Errorf("(%d, %d): unknown segment prefix: %s", seg.GetLine(), seg.GetChar(),
						seg.GetValue())
				}
				bytes = append(bytes, segPrefixes[seg.GetValue()])
			}
		}

		// #4 OpCode | 1 byte
		if i.packedRegister {
			regToken := l.GetOperands()[0].GetToken()
			if _, ok := regCodes[regToken.GetValue()]; !ok {
				return nil, fmt.Errorf("(%d, %d): unknown register %s", regToken.GetLine(),
					regToken.GetChar(), regToken.GetValue())
			}
			bytes = append(bytes, i.opCode | regCodes[regToken.GetValue()])
		} else {
			bytes = append(bytes, i.opCode)
		}

		// #5.1 ModRM | [0-1] bytes
		if i.modSet {
			modRM := byte(0)
			sibSet := false
			dispSet := false
			sib := byte(0)
			var disp []byte

			dest := l.GetOperands()[i.dest]
			if opType := dest.GetOperandType(); opType == lexeme.Mem8 || opType == lexeme.Mem16 ||
				opType == lexeme.Mem32 || opType == lexeme.Ptr16 {
				if opType != lexeme.Ptr16 {
					var v types.Variable
					for _, varLex := range l.GetParentProgram().GetVariables() {
						if dest.GetToken().GetValue() == varLex.GetName().GetValue() {
							v = varLex
						}
					}

					if v == nil {
						tok := dest.GetToken()
						return nil, fmt.Errorf("(%d, %d): unknown variable: %s", tok.GetLine(), tok.GetChar(),
							tok.GetValue())
					}

					// #5.3 Displacement | 4 bytes
					dispSet = true
					disp = append(disp, Int32ToBytes(v.GetName().GetLexeme().GetOffset())...)

					if dest.GetSumFirstToken() != nil && dest.GetSumSecondToken() != nil {
						if _, ok := regCodes[dest.GetSumFirstToken().GetValue()]; !ok {
							tok := dest.GetToken()
							return nil, fmt.Errorf("(%d, %d): unknown register: %s", tok.GetLine(),
								tok.GetChar(), tok.GetValue())
						}
						if _, ok := regCodes[dest.GetSumSecondToken().GetValue()]; !ok {
							tok := dest.GetToken()
							return nil, fmt.Errorf("(%d, %d): unknown register: %s", tok.GetLine(), tok.GetChar(),
								tok.GetValue())
						}
						modRM |= NewModRM(ModMemPlusDisp32, 0, RMSib)

						// #5.2 SIB | 1 byte
						sibSet = true
						sib |= NewSib(0, regCodes[dest.GetSumSecondToken().GetValue()],
							regCodes[dest.GetSumFirstToken().GetValue()])
					}
				} else {
					// displacement only mode
					modRM |= NewModRM(0, 0, RMDisp)
				}
			} else {
				tok := dest.GetToken()
				if _, ok := regCodes[tok.GetValue()]; !ok {
					return nil, fmt.Errorf("(%d, %d): unknown register: %s", tok.GetLine(), tok.GetChar(),
						tok.GetValue())
				}
				modRM |= NewModRM(ModReg, 0, regCodes[tok.GetValue()])
			}

			if i.regOpCodeExtensionSet {
				modRM |= NewModRM(0, i.regOpCodeExtension, 0)
			} else if len(l.GetOperands()) != 1 { // if has source
				var tok types.Token
				tok = l.GetOperands()[i.source].GetToken()

				if _, ok := regCodes[tok.GetValue()]; !ok {
					return nil, fmt.Errorf("(%d, %d): unknown register: %s", tok.GetLine(), tok.GetChar(),
						tok.GetValue())
				}
				modRM |= NewModRM(0, regCodes[tok.GetValue()], 0)
			}

			bytes = append(bytes, modRM)

			if sibSet {
				bytes = append(bytes, sib)
			}

			if dispSet {
				bytes = append(bytes, disp...)
			}
		}

		// #6 IMM | [0, 1, 2, 4] bytes
		if i.name == "jng" || i.name == "jmp" {
			op := l.GetOperands()[0]

			var label types.Token
			for _, lbl := range l.GetParentProgram().GetLabels() {
				if lbl.GetValue() == op.GetToken().GetValue() {
					label = lbl
					break
				}
			}

			if label != nil {
				currentOffset := l.GetOffset()
				lblOffset := label.GetLexeme().GetOffset()
				diff := lblOffset - currentOffset

				var imm []byte
				if i.name == "jng" {
					if op.GetOperandType() == lexeme.LabelForward {
						imm = append(imm, byte(diff - 2) & 0xFF, 0x90, 0x90, 0x90, 0x90)
					} else {
						imm = append(imm, byte(diff - GetSize(l)) & 0xFF, 0x90, 0x90, 0x90)
					}
				} else {
					if op.GetOperandType() == lexeme.LabelForward {
						imm = append(imm, byte(diff - 2) & 0xFF, 0x90, 0x90, 0x90)
					} else {
						imm = append(imm, Int32ToBytes(diff - GetSize(l))...)
					}
				}
				bytes = append(bytes, imm...)
			}

		}
		if i.immSet {
			for _, op := range l.GetOperands() {
				switch op.GetOperandType() {
				case lexeme.Constant8:
					bytes = append(bytes, byte(op.GetToken().GetNumValue()) & 0xFF)
				case lexeme.Constant32:
					bytes = append(bytes, Int32ToBytes(int(op.GetToken().GetNumValue()))...)
				}
			}
		}
	} else {
		directive := l.GetInstructionToken()
		value := l.GetOperands()[0].GetToken().GetNumValue()

		switch directive.GetValue() {
		case "db":
			bytes = append(bytes, byte(value) & 0xFF)
		case "dw":
			bytes = append(bytes, byte(value >> 8) & 0xFF, byte(value) & 0xFF)
		default:
			bytes = append(bytes, Int32ToBytes(int(value))...)
		}
	}
	return
}

func GetSize(l types.Lexeme) int {
	size := 0
	if _inst := l.GetInstruction(); _inst != nil {
		inst := _inst.(*instruction)
		// reserve 4 bytes for jmp forward, hardcode
		if l.GetInstructionToken().GetValue() == "jmp" {
			op := l.GetOperands()[0]
			var label types.Token
			for _, l := range l.GetParentProgram().GetLabels() {
				if op.GetToken().GetValue() == l.GetValue() {
					label = l
				}
			}
			if label != nil && label.GetLine() > op.GetToken().GetLine() {
				size += 4
				// opCode
				size++
			}
			return size
		}
		// opCode
		size++
		if inst.expansionPrefix {
			size++
		}
		if inst.operandSizePrefix {
			size++
		}
		if inst.modSet {
			size++
			if inst.sibSet {
				// SIB byte
				size++
				// btw this is also hardcoded as I don't have any displacements
				// :3
			}
		}

		size += int(inst.immSize)
		size += int(inst.disp)

		for _, op := range l.GetOperands() {
			if op.GetSegmentPrefix() != nil && op.GetSegmentPrefix().GetValue() != "ds" {
				size++
			}
		}
	} else {
		directive := l.GetInstructionToken()
		switch directive.GetValue() {
			case "db": size++; break
			case "dw": size += 2; break
			case "dd":
			default: size += 4; break
		}
	}
	return size
}

func (i *instruction) GetOpTypes() []int {
	return i.opTypes
}

func (i *instruction) SetName(value string) {
	i.name = value
}

func (i *instruction) SetOpTypes(value []int) {
	i.opTypes = value
}

func (i *instruction) CheckOpRestrictions(l types.Lexeme) error {
	if i.opRestrictions == nil {
		return nil
	}

	for i, restriction := range i.opRestrictions {
		if restriction != "" {
			token := l.GetOperands()[i].GetToken()
			if token.GetValue() != restriction {
				return fmt.Errorf("expected another operand %s", token.GetValue())
			}
		}
	}

	return nil
}

type InstructionOption func(*instruction) error

func FindInfo(l types.Lexeme) types.Instruction {
	for _, i := range instructions {
		name := l.GetInstructionToken()

		if name.GetValue() != i.GetName() {
			continue
		}

		if len(l.GetOperands()) != len(i.GetOpTypes()) {
			continue
		}

		wrong := false
		for index, op := range i.GetOpTypes() {
			// scale constant8 to constant32
			lOp := l.GetOperands()[index]
			if (lOp.GetOperandType() == lexeme.Constant8 || lOp.GetOperandType() == lexeme.Constant16) &&
				op == lexeme.Constant32 {
				lOp.SetOperandType(lexeme.Constant32)
			}
			if lOp.GetOperandType() != op {
				wrong = true
				break
			}
		}
		if wrong {
			continue
		}

		return i
	}
	return nil
}

var instructions = []types.Instruction{
	// 9F | LAHF | lahf | 2
	NewInstruction("lahf", 0x9F, []int{}),

	// 40+rw | INC r16 | inc ax | 1
	NewInstruction("inc", 0x40, []int{lexeme.Register16}, packedRegister(true), operandSizePrefix()),

	// 40+rw | INC r32 | inc eax | 1
	NewInstruction("inc", 0x40, []int{lexeme.Register32}, packedRegister(true)),

	// FF /1 | DEC r/m16 | dec word ptr [eax + edi*4] | 1/3
	NewInstruction("dec", 0xFF, []int{lexeme.Ptr16}, setModRM(), firstDest(), regOpCodeExtension(1),
		operandSizePrefix()),

	// 01 /r | ADD r/m32, r32 | add eax, ebx | 1/3
	// or
	// 03 /r | ADD r32, r/m32 | add edi, a[ebx + esi*4] | 1/2
	// but in reality we are making ADD r32, r32
	NewInstruction("add", 0x01, []int{lexeme.Register32, lexeme.Register32}, firstDest(), setModRM()),

	// 8D /r | LEA r32, ea32 | lea ebx, ES:Bin1 | 1
	NewInstruction("lea", 0x8D, []int{lexeme.Register32, lexeme.Mem32}, firstSource(), setModRM(), dispSize(4)),

	// 83 /4 ib | AND r/m16, imm8 | and Dec1, 02h | 1/3
	NewInstruction("and", 0x83, []int{lexeme.Mem16, lexeme.Constant8}, firstDest(), regOpCodeExtension(4),
		operandSizePrefix(), setModRM(), dispSize(4), immSize(1)),

	// B8+rd | MOV r/m32, imm32 | mov eax, 0001b | 1
	NewInstruction("mov", 0xB8, []int{lexeme.Register32, lexeme.Constant32}, firstDest(), packedRegister(true),
		immSize(4)),

	// 7E cb | JNG rel8 | 1
	NewInstruction("jng", 0x7E | 0b1, []int{lexeme.LabelBackward}, dispSize(1)),
	// OF 8E cw | JNG rel16 | 1
	NewInstruction("jng", 0x8E, []int{lexeme.LabelForward}, dispSize(4), expansionPrefix()),
	// EB cb | JMP rel8 | 1
	NewInstruction("jmp", 0xEB, []int{lexeme.LabelForward}, dispSize(1)),

	// 84 /r | TEST r/m8, r8 | test ES:Hex1, bh
	NewInstruction("test", 0x84 | 0b1, []int{lexeme.Mem8, lexeme.Register8}, setModRM(), firstDest(), dispSize(4)),
}

func regOpCodeExtension(regExt byte) InstructionOption {
	return func(i *instruction) error {
		if regExt > 0b111 {
			return fmt.Errorf("bad regExt passed, %b", regExt)
		}
		i.modSet = true
		i.regOpCodeExtensionSet = true
		i.regOpCodeExtension = regExt
		return nil
	}
}

func setModRM() InstructionOption {
	return func(i *instruction) error {
		i.modSet = true
		return nil
	}
}


func dispSize(value byte) InstructionOption {
	return func(i *instruction) error {
		i.disp = value
		return nil
	}
}

func immSize(value byte) InstructionOption {
	return func(i *instruction) error {
		if value > 4 {
			return fmt.Errorf("immSize: more than 4 bytes are not suppoted")
		}
		i.immSet = true
		i.immSize = value
		return nil
	}
}

func packedRegister(value bool) InstructionOption {
	return func(i *instruction) error {
		i.packedRegister = value
		return nil
	}
}

func opRestrictions(value []string) InstructionOption {
	return func(i *instruction) error {
		i.opRestrictions = value
		return nil
	}
}

func operandSizePrefix() InstructionOption {
	return func(i *instruction) error {
		i.operandSizePrefix = true
		return nil
	}
}

func expansionPrefix() InstructionOption {
	return func(i *instruction) error {
		i.expansionPrefix = true
		return nil
	}
}

func firstSource() InstructionOption {
	return func(i *instruction) error {
		i.source = 0
		i.dest = 1
		return nil
	}
}

func firstDest() InstructionOption {
	return func(i *instruction) error {
		i.source = 1
		i.dest = 0
		return nil
	}
}

func NewInstruction(name string, opCode byte, opTypes []int, opts ...InstructionOption) types.Instruction {
	i := &instruction{name: name, opCode: opCode, opTypes: opTypes}
	for _, opt := range opts  {
		if err := opt(i); err != nil {
			panic(err)
		}
	}
	return i
}
