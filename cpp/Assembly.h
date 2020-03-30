#pragma once

#include "Delimiter.h"

enum UserNameType
{
   NT_Label,
   NT_Var,
   NT_Segment
};

struct UserName
{
   UserNameType type;

   end_token token;

   int offset {0};
   int begin {0};
   int end {0};
};

extern vector<UserName> userNames;

UserName* getUserName(UserNameType type, const string& name);
UserName* getUserName(UserNameType type, int line);

void printLexemeList();
void outputErrors();

void analyzeSegments();
void analyzeVariablesAndLabels();
void analyzeOperandTypes();