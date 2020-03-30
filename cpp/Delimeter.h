#pragma once

#include "TokenType.h"

using namespace std;

//keep information about tokens
struct end_token
{
   string token;
   TokenType type;
};

struct Lexem
{
   int instrIndex = 0;
   int operandIndices[50];
   int operandLengths[50];
   bool hasLabel = false;
   bool hasName = false;
   bool hasOperands = false;
   bool hasInstruction = false;
   bool hasMacro = false;
   int offset = 0;
   int numberOfOperands = 0;
};

struct macro
{
   end_token name;
   vector<end_token> parameters;
   int start;
   int end;
};

void createTokens(const string& filename);

