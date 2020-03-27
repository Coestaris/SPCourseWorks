package assembly

import (
	"course_work_2/lexeme"
	"course_work_2/types"
	"fmt"
)

const expansionCode = 0x0F

const (
	al = 0b000
	cl = 0b001
	dl = 0b010
	bl = 0b011
	ah = 0b100
	ch = 0b101
	dh = 0b110
	bh = 0b111

	ax = al
	cx = cl
	dx = dl
	bx = bl
	sp = ah
	bp = ch
	si = dh
	di = bh

	eax = al
	ecx = cl
	edx = dl
	ebx = bl
	esp = ah
	ebp = ch
	esi = dh
	edi = bh

	sib = 0b100
)

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
	modSet bool
	modRM byte

	// SIB section
	sibSet bool
	sib byte

	// packedRegister means that register info is packed inside last 3 bits of opcode
	packedRegister        bool
	packedSize            bool

	// Displacement
	disp    byte

	// Immediate value
	immSize    byte
	// Optional instruction prefixes
	expansionPrefix       bool
	addressSizePrefix     bool
	// Set this to true if we work with 16bit operand, if not = 8/32 bit
	operandSizePrefix     bool
	segmentOverridePrefix bool

	name               string
	opTypes            []int
	opRestrictions     []string
}

func (i *instruction) setSib(ss, index, base byte) error {
	if ss > 0b11 {
		return fmt.Errorf("sib: wrong ss passed, %b", ss)
	}
	if index > 0b111 {
		return fmt.Errorf("sib: wrong index passed, %b", index)
	}
	if base > 0b111 {
		return fmt.Errorf("sib: wrong base passed, %b", base)
	}
	i.sib = ss << 6 | index << 3 | base
	return nil
}

func (i *instruction) setMod(mod, reg, rm byte) error {
	if mod > 0b11 {
		return fmt.Errorf("mod: wrong mod passed, %b", mod)
	}
	if rm > 0b111 {
		return fmt.Errorf("mod: wrong index passed, %b", reg)
	}
	if reg > 0b111 {
		return fmt.Errorf("rm: wrong base passed, %b", rm)
	}
	i.modRM = mod << 6 | reg << 3 | rm
	return nil
}

func (i *instruction) GetName() string {
	return i.name
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

		/*for _, op := range l.GetOperands() {
			if op.GetSegmentPrefix() != nil {
				size++
			}
		}*/
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
			if lOp.GetOperandType() == lexeme.Constant8 && op == lexeme.Constant32 {
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
	NewInstruction("inc", 0x40, []int{lexeme.Register16}, packedRegister(true),
		operandSizePrefix()),

	// FF /1 | DEC r/m16 | dec word ptr [eax + edi*4] | 1/3
	NewInstruction("dec", 0xFF, []int{lexeme.Mem}, setSIB(), regOpCodeExtension(1),
		operandSizePrefix()),

	// 01 /r | ADD r/m32, r32 | add eax, ebx | 1/3
	// or
	// 03 /r | ADD r32, r/m32 | add edi, a[ebx + esi*4] | 1/2
	// but in reality we are making ADD r32, r32
	NewInstruction("add", 0x01, []int{lexeme.Register32, lexeme.Register32}, setModRM()),

	// 8D /r | LEA r32, ea32 | lea ebx, ES:Bin1 | 1
	NewInstruction("lea", 0x8D, []int{lexeme.Register32, lexeme.Mem}, setModRM(), dispSize(4)),

	// 83 /4 ib | AND r/m16, imm8 | and Dec1, 02h | 1/3
	NewInstruction("and", 0x83, []int{lexeme.Mem, lexeme.Constant8}, regOpCodeExtension(4),
		operandSizePrefix(), setModRM(), dispSize(4), immSize(1)),

	// B8+rd | MOV r/m32, imm32 | mov eax, 0001b | 1
	NewInstruction("mov", 0xB8, []int{lexeme.Register32, lexeme.Constant32}, packedRegister(true),
		immSize(4)),

	// 7E cb | JNG rel8 | 1
	NewInstruction("jng", 0x7E | 0b1, []int{lexeme.LabelBackward}, dispSize(1)),
	// OF 8E cw | JNG rel16 | 1
	NewInstruction("jng", 0x8E, []int{lexeme.LabelForward}, dispSize(4), expansionPrefix()),
	// EB cb | JMP rel8 | 1
	NewInstruction("jmp", 0xEB, []int{lexeme.LabelForward}, dispSize(1)),

	// 84 /r | TEST r/m8, r8 | test ES:Hex1, bh
	NewInstruction("test", 0x84 | 0b1, []int{lexeme.Mem, lexeme.Register8}, setModRM(), dispSize(4)),
}

func regOpCodeExtension(regExt byte) InstructionOption {
	return func(i *instruction) error {
		if regExt > 0b111 {
			return fmt.Errorf("bad regExt passed, %b", regExt)
		}
		i.modSet = true
		i.modRM = regExt << 3
		return nil
	}
}

func setSIB() InstructionOption {
	return func(i *instruction) error {
		// set modRM:reg to sib
		i.modSet = true
		i.sibSet = true
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

func NewInstruction(name string, opCode byte, opTypes []int, opts ...InstructionOption) types.Instruction {
	i := &instruction{name: name, opCode: opCode, opTypes: opTypes}
	for _, opt := range opts  {
		if err := opt(i); err != nil {
			panic(err)
		}
	}
	return i
}
