#pragma once

#include "TokenType.h"

using namespace std;

//keep information about tokens
struct end_token
{
   string token;
   TokenType type;
};

enum operandType
{
   OT_Const8,
   OT_Const32,
   OT_Register8,
   OT_Register32,
   OT_LabelFwd,
   OT_LabelBack,
   OT_Memory,
   OT_Memory8,
   OT_Memory32,
};

struct Lexem
{
   string error;
   end_token error_token;
   bool has_error;

   int instrIndex = 0;

   // Operand info
   int operandIndices[5];
   int operandLengths[5];
/*   operandType operandTypes[5];
   bool has_segment_prefix;
   end_token segment_prefix;*/

   bool hasLabel = false;
   bool hasName = false;
   bool hasOperands = false;
   bool hasInstruction = false;
   bool hasMacro = false;
   int offset = 0;
   int numberOfOperands = 0;

   void SetError(string error, const end_token& end_token);
};

struct macro
{
   end_token name;
   vector<end_token> parameters;
   int start;
   int end;
};

extern vector<vector<end_token>> vectorOfTokens;
extern vector<Lexem> lexems;

void createTokens(const string& filename);

