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

	WORDPTR

	PTR

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

	"WORDPTR",

	"PTR",

	"IDENTIFIER",

	"LABEL",
}

// Reserved tokens
var tokenSymbols = []string{"[", "]", ".", ",", "+", "-", ":", "*"}
var tokenRegister32s = []string{"eax", "ebx", "ecx", "edx", "edi", "esi"}
var tokenRegister16s = []string{"ax", "bx", "cx", "dx", "sp", "bp", "si", "di"}
var tokenRegister8s = []string{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}
var tokenSegRegs = []string{"es", "cs", "ss", "ds", "fs", "gs"}
var tokenDirectives = []string{"db", "dw", "dd"}
var tokenInstructions = []string{"lahf", "inc", "dec", "add", "jng", "lea", "and", "mov", "test", "jmp"}
var tokenModel = []string{"model"}
var tokenWord = []string{"word"}
var tokenPtr = []string{"ptr"}
var tokenDataSegment = []string{"data"}
var tokenCodeSegment = []string{"code"}
var tokenEnd = []string{"end"}

type tokenRule struct {
	rule []string
	tokenType int
}

var numberHexRegex, _ = regexp.Compile("^-?[0-9a-f]+h$")
var numberDecRegex, _ = regexp.Compile("^-?[0-9]+$")
var numberBinRegex, _ = regexp.Compile("^-?[01]+b$")
var identifierRegex, _ = regexp.Compile(`^[a-z]\w*$`)

var TokenDict = []tokenRule{
	{tokenSymbols, SYM},
	{tokenRegister8s, REG8},
	{tokenRegister16s, REG16},
	{tokenRegister32s, REG32},
	{tokenSegRegs, SEGREG},
	{tokenDirectives, DIRECTIVE},
	{tokenInstructions, INSTRUCTION},
	{tokenModel, MODEL},
	{tokenWord, WORDPTR},
	{tokenPtr, PTR},
	{tokenDataSegment, DATA},
	{tokenCodeSegment, CODE},
	{tokenEnd, END},
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