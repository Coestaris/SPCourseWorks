#include"TokenType.h"
#include"Delimiter.h"
#include"Lexem.h"
#include "Assembly.h"

int main()
{
	setupTokenTypeDict();

	// ET2 routines
	createTokens("test.txt");
	proceedTokens();
	//printTokenList();

	// ET3 routines
	analyzeSegments();
	analyzeVariablesAndLabels();
	analyzeOperandTypes();
	checkInsrtuctionRequirements();
	calculateSize();

	printLexemeList();

	outputErrors();

	return 0;
}