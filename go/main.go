package main

import (
	"course_work_2/assembly"
	"course_work_2/tokens"
	"course_work_2/types"
	"fmt"
	log "github.com/sirupsen/logrus"
	flag "github.com/spf13/pflag"
	"io/ioutil"
	"os"
	"strconv"
	"strings"
)

var compileFiles []string

func init() {
	flag.StringArrayVarP(&compileFiles, "compile", "c", []string{}, ".asm to compile")

	flag.Parse()
}

func rjust(pad int, str string, padString string) string {
	if len(str) < pad {
		buf := ""
		for i := 0; i < pad - len(str); i++ {
			buf += padString
		}
		str = buf + str
	}
	return str
}

func printLexemes(l types.Lexeme) {
	for i, t := range l.GetTokens() {
		fmt.Printf("%s. %s\n", rjust(2, strconv.Itoa(i), "0"), t.ToString())
	}
}

func PrintET2(program types.ASM) (res string) {
	for _, l := range program.GetLexemes() {
		sources := make([]string, len(l.GetTokens()))
		for i, t := range l.GetTokens() {
			sources[i] = t.GetValue()
		}
		res += fmt.Sprintf("Source : %s\n", strings.Join(sources, " "))
		res += fmt.Sprintf("Lexemes: \n")
		printLexemes(l)
		res += fmt.Sprintln("+-----------------------------------------+")
		res += fmt.Sprintln("|  LABEL |  MNEM  |   Op1  |   Op2  | ....|")
		res += fmt.Sprintln("|-----------------------------------------|")
		res += fmt.Sprintln("|        |  I | L |  I | L |  I | L |     |")
		res += fmt.Sprintln("|-----------------------------------------|")
		res += fmt.Sprintf("|  %s\n", l.ToSentenceTableString(0))
		res += fmt.Sprintln("+-----------------------------------------+")
		res += fmt.Sprintln()
		res += fmt.Sprintln()
	}
	return res
}

func PrintET3(a types.ASM) (res string) {
	res += fmt.Sprintln("+---------------------------------------------+")
	res += fmt.Sprintln("| № | OFFSET  |   SOURCE                      |")
	res += fmt.Sprintln("+---------------------------------------------+")
	for i, l := range a.GetLexemes() {
		offsetHex := fmt.Sprintf("%x", l.GetOffset())
		inst := l.GetInstructionToken()
		if inst.GetTokenType() == tokens.END {
			offsetHex = "----"
		}

		res += fmt.Sprintf("|%s|     %s|%s\n", rjust(3, strconv.Itoa(i), "0"), rjust(4, offsetHex, "0"),
			l.PrettyPrint())
	}
	res += fmt.Sprintln("+---------------------------------------------+")

	res += fmt.Sprintln()

	res += fmt.Sprintln("+------------------------------------------+")
	res += fmt.Sprintln("|------------SEGMENTS TABLE----------------|")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintln("| № |    SEG NAME   | BIT DEPTH |  OFFSET  |")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintf("|%s|           DATA|         32|%s|\n", rjust(3, "1", "0"),
		rjust(10, fmt.Sprintf("%x", a.GetDataSegment().GetSize()), " "))
	res += fmt.Sprintf("|%s|           CODE|         32|%s|\n", rjust(3, "1", "0"),
		rjust(10, fmt.Sprintf("%x", a.GetCodeSegment().GetSize()), " "))
	res += fmt.Sprintln("+------------------------------------------+")

	res += fmt.Sprintln()

	res += fmt.Sprintln("+------------------------------------------+")
	res += fmt.Sprintln("|------------USER DEFINED NAMES------------|")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintln("| № |   NAME   |  TYPE  |  SEG  |  OFFSET  |")
	res += fmt.Sprintln("|------------------------------------------|")
	i := 0
	for _, l := range a.GetLabels() {
		res += fmt.Sprintf("|%s|%s|  LABEL |%s|%s|\n", rjust(3, strconv.Itoa(-i), "0"),
			rjust(10, l.GetValue(), " "),
			rjust(7, l.GetLexeme().GetSegment().GetOpen().GetValue(), " "),
			rjust(10, fmt.Sprintf("%x", l.GetLexeme().GetOffset()), " "))
	}
	res += fmt.Sprintln("+------------------------------------------+")

	res += fmt.Sprintln()

	res += fmt.Sprintln("+------------------------------------------+")
	res += fmt.Sprintln("|--------SEG REGISTERS DESTINATIONS--------|")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintln("| № |          REGISTER         |   DEST   |")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintf("| 0 |                         DS|   %s   |\n", a.GetDataSegment().GetOpen().GetValue())
	res += fmt.Sprintf("| 1 |                         CS|   %s   |\n", a.GetCodeSegment().GetOpen().GetValue())
	res += fmt.Sprintln("| 2 |                         SS|   NONE   |")
	res += fmt.Sprintln("| 3 |                         ES|   NONE   |")
	res += fmt.Sprintln("| 4 |                         GS|   NONE   |")
	res += fmt.Sprintln("| 5 |                         FS|   NONE   |")
	res += fmt.Sprintln("+------------------------------------------+")

	return res
}

func PrintET4(asm types.ASM, bytes [][]byte) (res string) {
	byteIndex := 0
	for i, l := range asm.GetLexemes() {
		hexOffset := fmt.Sprintf("%x", l.GetOffset())
		inst := l.GetInstructionToken()
		if inst.GetTokenType() == tokens.END {
			hexOffset = "----"
		}

		if l.GetInstruction() != nil {
			bytesStr := ""
			for _, b := range bytes[byteIndex] {
				pad := ""
				if b < 0x10 {
					pad = "0"
				}
				bytesStr += fmt.Sprintf("%s%X ", pad, b)
			}
			res += fmt.Sprintf("| %s | %2s | %25s | %s\n", rjust(2, strconv.Itoa(i), "0"),
				hexOffset, bytesStr, l.PrettyPrint())
			byteIndex++
		}
	}
	return
}

func main() {
	if len(compileFiles) == 0 {
		if _, err := fmt.Fprint(os.Stderr, "-c/--compile flag is required"); err != nil {
			panic(err)
		}
		return
	}

	for _, fileURL := range compileFiles {
		parsedFile, err := ioutil.ReadFile(fileURL)
		if err != nil {
			log.Fatal(err)
		}

		program := assembly.NewAssembly(string(parsedFile), fileURL)
		if errors := program.Parse(); errors != nil {
			for _, err := range errors {
				log.Error(err)
			}
			log.Fatal("parsing failed")
		}

		// fmt.Println(PrintET2(program))

		if errors := program.FirstPass(); errors != nil {
			for _, err := range errors {
				log.Error(err)
			}
			log.Fatal("first pass failed")
		}

		if err := ioutil.WriteFile("generated.lst", []byte(PrintET3(program)), 0644); err != nil {
			panic(err)
		}

		if bytes, errors := program.SecondPass(); errors != nil {
			for _, err := range errors {
				log.Error(err)
			}
			log.Fatal("second pass failed")
		} else {
			fmt.Println(PrintET4(program, bytes))
		}
	}
}