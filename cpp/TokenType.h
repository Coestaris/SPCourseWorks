#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string.h>
#include<string>
#include <unordered_map>
#include<regex>
#include<vector>

namespace KeyWord
{
    enum  TokenType
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


        Unknown
    };
}

using namespace std;
using namespace KeyWord;
string TokenTypeToString(TokenType tochange);
void input();
void output();
TokenType numcheck(string number);
