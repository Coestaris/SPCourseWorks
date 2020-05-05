#include "TokenType.h"
#include "Delimiter.h"
#include "Lexem.h"
#include "Assembly.h"
#include "Bytes.h"

int main()
{
	setupTokenTypeDict();
	setupByteDict();

	// ET2 routines
	createTokens("test.txt");
	proceedTokens();
	//printTokenList();

	// ET3 routines
	analyzeSegments();
	analyzeVariablesAndLabels();
	analyzeOperandTypes();
    checkInstructionRequirements();
	calculateSize();
	//printLexemeList();

	// ET4 routines
	getBytes();
	printBytes();

	printTables();

	outputErrors();

	return 0;
}