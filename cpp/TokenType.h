#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <unordered_map>
#include <regex>
#include <vector>

enum TokenType
{

   DbDirective,
   DwDirective,
   DdDirective,


   Instruction,
   SegmentKeyword,
   MacroKeyword,

   PtrKeyword,
   WordKey,
   DwKey,
   ByteKey,

   EndKeyword,
   EndmKeyword,
   EndsKeyword,

   Register16,
   Register8,

   Symbol,

   Identifier,

   HexNumber,
   DecNumber,
   BinNumber,
   Text,

   UserSegment,
   SegmentRegister,
   Label,
   MacroName,

   Unknown
};

using namespace std;

string tokenTypeToString(TokenType type);
void setupTokenTypeDict();
void printTokenList();
