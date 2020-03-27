package main

import (
	"course_work_2/assembly"
	"course_work_2/tokens"
	"course_work_2/types"
	"fmt"
	flag "github.com/spf13/pflag"
	"io/ioutil"
	"log"
	"os"
	"strconv"
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

func print3(program types.ASM) {
	fmt.Println("+---------------------------------------------+")
	fmt.Println("| № | OFFSET  |   SOURCE                      |")
	fmt.Println("+---------------------------------------------+")
	for i, l := range program.GetLexemes() {
		offsetHex := fmt.Sprintf("%x", l.GetOffset())
		inst := l.GetInstructionToken()
		if inst.GetTokenType() == tokens.END {
			offsetHex = "----"
		}

		fmt.Printf("|%s|     %s|%s\n", rjust(3, strconv.Itoa(i), "0"), rjust(4, offsetHex, "0"),
			l.PrettyPrint())
	}
	fmt.Println("+---------------------------------------------+")

	fmt.Println()

	fmt.Println("+------------------------------------------+")
	fmt.Println("|------------SEGMENTS TABLE----------------|")
	fmt.Println("|------------------------------------------|")
	fmt.Println("| № |    SEG NAME   | BIT DEPTH |  OFFSET  |")
	fmt.Println("|------------------------------------------|")
	fmt.Printf("|%s|           DATA|         32|%s|\n", rjust(3, "1", "0"),
		rjust(10, fmt.Sprintf("%x", program.GetDataSegment().GetSize()), " "))
	fmt.Printf("|%s|           CODE|         32|%s|\n", rjust(3, "1", "0"),
		rjust(10, fmt.Sprintf("%x", program.GetCodeSegment().GetSize()), " "))
	fmt.Println("+------------------------------------------+")

	fmt.Println()

	fmt.Println("+------------------------------------------+")
	fmt.Println("|------------USER DEFINED NAMES------------|")
	fmt.Println("|------------------------------------------|")
	fmt.Println("| № |   NAME   |  TYPE  |  SEG  |  OFFSET  |")
	fmt.Println("|------------------------------------------|")
	i := 0
	for _, l := range program.GetLabels() {
		fmt.Printf("|%s|%s|  LABEL |%s|%s|\n", rjust(3, strconv.Itoa(-i), "0"),
			rjust(10, l.GetValue(), " "),
			rjust(7, l.GetLexeme().GetSegment().GetOpen().GetValue(), " "),
			rjust(10, fmt.Sprintf("%x", l.GetLexeme().GetOffset()), " "))
	}
	fmt.Println("+------------------------------------------+")

	fmt.Println()

	fmt.Println("+------------------------------------------+")
	fmt.Println("|--------SEG REGISTERS DESTINATIONS--------|")
	fmt.Println("|------------------------------------------|")
	fmt.Println("| № |          REGISTER         |   DEST   |")
	fmt.Println("|------------------------------------------|")
	fmt.Printf("| 0 |                         DS|   %s   |\n", program.GetDataSegment().GetOpen().GetValue())
	fmt.Printf("| 1 |                         CS|   %s   |\n", program.GetCodeSegment().GetOpen().GetValue())
	fmt.Println("| 2 |                         SS|   NONE   |")
	fmt.Println("| 3 |                         ES|   NONE   |")
	fmt.Println("| 4 |                         GS|   NONE   |")
	fmt.Println("| 5 |                         FS|   NONE   |")
	fmt.Println("+------------------------------------------+")
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
		if err = program.Parse(); err != nil {
			log.Fatal(err)
		}
		if err = program.FirstPass(); err != nil {
			log.Fatal(err)
		}

		print3(program)
	}
}