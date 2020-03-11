
#pragma once

#include<iostream>
#include<string.h>
#include<string>
#include <map>
using namespace std;



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


using namespace KeyWord;
string TokenTypeToString(TokenType sex);
void input();
void output();
bool contains(char s);
void delimeter();
TokenType numcheck(string number);