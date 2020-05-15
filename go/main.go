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
	"strings"
)

var compileFiles []string
var saveDest string
var et2 bool
var et3 bool
var et4 bool

func init() {
	flag.StringArrayVarP(&compileFiles, "compile", "c", []string{}, ".asm to compile")
	flag.StringVarP(&saveDest, "save", "s", "res.lst", "location to save")
	flag.BoolVar(&et2, "et2", true, "save eta2 info")
	flag.BoolVar(&et3, "et3", true, "save eta3 info")
	flag.BoolVar(&et4, "et4", true, "save eta4 info")

	flag.Parse()
}

func printLexemes(l types.Lexeme) (res string) {
	for i, t := range l.GetTokens() {
		res += fmt.Sprintf("%02d. %s\n", i, t.ToString())
	}
	return
}

func PrintET2(program types.ASM) (res string) {
	for _, l := range program.GetLexemes() {
		sources := make([]string, len(l.GetTokens()))
		for i, t := range l.GetTokens() {
			sources[i] = t.GetValue()
		}
		res += fmt.Sprintf("Source : %s\n", strings.Join(sources, " "))
		res += fmt.Sprintf("Lexemes: \n")
		res += printLexemes(l)
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
		offsetHex := fmt.Sprintf("%04x", l.GetOffset())
		inst := l.GetInstructionToken()
		if inst.GetTokenType() == tokens.END {
			offsetHex = "----"
		}

		res += fmt.Sprintf("|%03d|     %s|%s\n", i, offsetHex,
			l.PrettyPrint())
	}
	res += fmt.Sprintln("+---------------------------------------------+")

	res += fmt.Sprintln()

	res += fmt.Sprintln("+------------------------------------------+")
	res += fmt.Sprintln("|------------SEGMENTS TABLE----------------|")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintln("| № |    SEG NAME   | BIT DEPTH |  OFFSET  |")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintf("|001|           DATA|         32|%10x|\n",
		a.GetDataSegment().GetSize())
	res += fmt.Sprintf("|002|           CODE|         32|%10x|\n",
		a.GetCodeSegment().GetSize())
	res += fmt.Sprintln("+------------------------------------------+")

	res += fmt.Sprintln()

	res += fmt.Sprintln("+------------------------------------------+")
	res += fmt.Sprintln("|------------USER DEFINED NAMES------------|")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintln("| № |   NAME   |  TYPE  |  SEG  |  OFFSET  |")
	res += fmt.Sprintln("|------------------------------------------|")
	i := 0
	for _, l := range a.GetLabels() {
		res += fmt.Sprintf("|%03d|%10s|  LABEL |%7s|%10x|\n", -i,
			l.GetValue(),
			l.GetLexeme().GetSegment().GetOpen().GetValue(),
			l.GetLexeme().GetOffset())
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

func PrintET4(a types.ASM, bytes [][]byte) (res string) {
	byteIndex := 0
	for i, l := range a.GetLexemes() {
		hexOffset := fmt.Sprintf("%02x", l.GetOffset())
		inst := l.GetInstructionToken()
		if inst.GetTokenType() == tokens.END {
			hexOffset = "--"
		}

		bytesStr := ""
		if l.GetInstruction() != nil {
			for _, b := range bytes[byteIndex] {
				bytesStr += fmt.Sprintf("%02X ", b)
			}
			byteIndex++
		}
			res += fmt.Sprintf("| %02d | %2s | %-25s | %s\n", i,
				hexOffset, bytesStr, l.PrettyPrint())
	}
	res += fmt.Sprintln("+---------------------------------------------+")

	res += fmt.Sprintln()

	res += fmt.Sprintln("+------------------------------------------+")
	res += fmt.Sprintln("|------------SEGMENTS TABLE----------------|")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintln("| № |    SEG NAME   | BIT DEPTH |  OFFSET  |")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintf("|001|           DATA|         32|%10x|\n",
		a.GetDataSegment().GetSize())
	res += fmt.Sprintf("|002|           CODE|         32|%10x|\n",
		a.GetCodeSegment().GetSize())
	res += fmt.Sprintln("+------------------------------------------+")

	res += fmt.Sprintln()

	res += fmt.Sprintln("+------------------------------------------+")
	res += fmt.Sprintln("|------------USER DEFINED NAMES------------|")
	res += fmt.Sprintln("|------------------------------------------|")
	res += fmt.Sprintln("| № |   NAME   |  TYPE  |  SEG  |  OFFSET  |")
	res += fmt.Sprintln("|------------------------------------------|")
	i := 0
	for _, l := range a.GetLabels() {
		res += fmt.Sprintf("|%03d|%10s|  LABEL |%7s|%10x|\n", -i,
			l.GetValue(),
			l.GetLexeme().GetSegment().GetOpen().GetValue(),
			l.GetLexeme().GetOffset())
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

		if et2 {
			if err := ioutil.WriteFile(saveDest, []byte(PrintET2(program)), 0644); err != nil {
				panic(err)
			}
		}

		if errors := program.FirstPass(); errors != nil {
			for _, err := range errors {
				log.Error(err)
			}
			log.Fatal("first pass failed")
		}

		if et3 {
			if err := ioutil.WriteFile(saveDest, []byte(PrintET3(program)), 0644); err != nil {
				panic(err)
			}
		}

		if bytes, errors := program.SecondPass(); errors != nil {
			for _, err := range errors {
				log.Error(err)
			}
			log.Fatal("second pass failed")
		} else {
			if et4 {
				if err := ioutil.WriteFile(saveDest, []byte(PrintET4(program, bytes)), 0644);
					err != nil {
					panic(err)
				}
			}
		}
	}
}