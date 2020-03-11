package tokens

import "regexp"

const (
	SYM = iota

	REG8
	REG16
	REG32

	DIRECTIVE
	INSTRUCTION

	MODEL
	DATA
	CODE
	END
	SEGREG

	HEX
	BIN
	DEC

	IDENTIFIER

	LABEL

	last
)

var tokenMap = []string{
	"SYM",

	"REG8",
	"REG16",
	"REG32",

	"DIRECTIVE",
	"INSTRUCTION",

	"MODEL",
	"DATA",
	"CODE",
	"END",
	"SEGREG",

	"HEX",
	"BIN",
	"DEC",

	"IDENTIFIER",

	"LABEL",
}

var TokenSymbols = []string{"[", "]", ".", ",", "+", "-", ":", "*"}
var TokenRegister32s = []string{"eax", "ebx", "ecx", "edx", "edi", "esi"}
var TokenRegister16s = []string{"ax", "bx", "cx", "dx", "sp", "bp", "si", "di"}
var TokenRegister8s = []string{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}
var TokenSegregs = []string{"es", "cs", "ss", "ds", "fs", "gs"}
var TokenDirectives = []string{"db", "dw", "dd"}
var TokenInstructions = []string{"cld", "dec", "inc", "cmp", "shl", "in", "add", "jnz", "jmp", "or"}

type tokenRule struct {
	rule []string
	tokenType int
}

var numberHexRegex, _ = regexp.Compile("^[0-9a-f]+h$")
var numberDecRegex, _ = regexp.Compile("^[0-9]+$")
var numberBinRegex, _ = regexp.Compile("^[01]+b$")
var identifierRegex, _ = regexp.Compile(`^[a-z]\w*$`)

var TokenDict = []tokenRule{
	{TokenSymbols, SYM},
	{TokenRegister8s, REG8},
	{TokenRegister16s, REG16},
	{TokenRegister32s, REG32},
	{TokenSegregs, SEGREG},
	{TokenDirectives, DIRECTIVE},
	{TokenInstructions, INSTRUCTION},
	{[]string{"model"}, MODEL},
	{[]string{"data"}, DATA},
	{[]string{"code"}, CODE},
	{[]string{"end"}, END},
}

func typeByValue(value string) int {
	for _, rule := range TokenDict {
		for _, entry := range rule.rule {
			if entry == value {
				return rule.tokenType
			}
		}
	}

	if numberDecRegex.MatchString(value) {
		return DEC
	} else if numberBinRegex.MatchString(value) {
		return BIN
	} else if numberHexRegex.MatchString(value) {
		return HEX
	} else if identifierRegex.MatchString(value) {
		return IDENTIFIER
	}

	return -1
}

func ToStringValue(tokenType int) string {
	if tokenType < 0 || tokenType > last - 1 {
		return "unknown"
	}
	return tokenMap[tokenType]
}