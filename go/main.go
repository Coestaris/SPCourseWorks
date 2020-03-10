package main

import (
	"course_work_2/assembly"
	"fmt"
	flag "github.com/spf13/pflag"
	"io/ioutil"
	"log"
	"os"
)

var compileFiles []string

func init() {
	flag.StringArrayVarP(&compileFiles, "compile", "c", []string{}, ".asm to compile")

	flag.Parse()
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

		fmt.Println("Tokens: ")
		for _, l := range *program.GetLexemes() {
			fmt.Println(l.ToString())
		}

		fmt.Println()
		fmt.Println()
		fmt.Println("Indexed tokens: ")
		fmt.Println(program.ToIndexedTable())

		fmt.Println()
		fmt.Println("Sentence table: ")
		fmt.Println(program.ToSentenceTable())
	}
}