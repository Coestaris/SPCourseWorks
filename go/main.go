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

func print2(program types.ASM) {
	for _, l := range program.GetLexemes() {
		sources := make([]string, len(l.GetTokens()))
		for i, t := range l.GetTokens() {
			sources[i] = t.GetValue()
		}
		fmt.Printf("Source : %s\n", strings.Join(sources, " "))
		fmt.Printf("Lexemes: \n")
		printLexemes(l)
		fmt.Println("+---------------------------------------+")
		fmt.Println("|  LBL |  MNEM  |   Op1  |   Op2  | ....|")
		fmt.Println("|      |  I | L |  I | L |  I | L |     |")
		fmt.Printf("|  %s\n", l.ToSentenceTableString(0))
		fmt.Println()
		fmt.Println()
	}
}

func print3(program types.ASM) {
	fmt.Println("+=============================================+")
	fmt.Println("| # | Offset  |   Source                      |")
	fmt.Println("+=============================================+")
	for i, l := range program.GetLexemes() {
		offset := strconv.Itoa(l.GetOffset())
		inst := l.GetInstructionToken()
		if inst.GetTokenType() == tokens.END {
			offset = "-----"
		}

		fmt.Printf("|%s|%s|%s\n", rjust(3, strconv.Itoa(i), "0"), rjust(9, offset, "-"),
			l.PrettyPrint())
	}

	fmt.Println("\n+------------------------------------------+")
	fmt.Println("|------------SEGMENTS TABLE----------------|")
	fmt.Println("| # | segment Name  | Bit depth |  Offset  |")
	fmt.Println("+------------------------------------------+")
	fmt.Printf("|%s| DATA          |    32     |%s|\n", rjust(3, "1", "0"),
		rjust(10, strconv.Itoa(program.GetDataSegment().GetSize()), "-"))
	fmt.Printf("|%s| CODE          |    32     |%s|\n", rjust(3, "1", "0"),
		rjust(10, strconv.Itoa(program.GetCodeSegment().GetSize()), "-"))
	fmt.Println("+------------------------------------------+\n")

	fmt.Println("+------------------------------------------+")
	fmt.Println("+------USER DEFINED NAMES------------------+")
	fmt.Println("+------------------------------------------+")
	i := 0
	for _, l := range program.GetLabels() {
		fmt.Printf("|%s|%s|   LABEL |%s|%s|\n", rjust(3, strconv.Itoa(i), "0"),
			rjust(10, l.GetValue(), " "),
			rjust(3, l.GetLexeme().GetSegment().GetOpen().GetValue(), " "),
			rjust(4, strconv.Itoa(l.GetLexeme().GetOffset()), " "))
	}
}

func printLexemes(l types.Lexeme) {
	for i, t := range l.GetTokens() {
		fmt.Printf("%s. %s\n", rjust(2, strconv.Itoa(i), "0"), t.ToString())
	}
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