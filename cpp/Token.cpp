#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string.h>
#include <unordered_map>
#include<regex>
#include<vector>
#include"TokenType.h"



unordered_map<string, TokenType >keyword;
unordered_map<string, TokenType>::iterator itr;

struct end_token
{
	string token;
	TokenType type;
	
};
vector<vector<end_token>> vector_of_token;


void input()
{
	keyword["db"] = DbDirective;
	keyword["dw"] = DwDirective;
	keyword["dd"] = DdDirective;
	keyword["segment"] = SegmentKeyword;
	keyword["aas"] = Instruction;
	keyword["inc"] = Instruction;
	keyword["neg"] = Instruction;
	keyword["bt"] = Instruction;
	keyword["and"] = Instruction;
	keyword["cmp"] = Instruction;
	keyword["mov"] = Instruction;
	keyword["or"] = Instruction;
	keyword["jl"] = Instruction;
	keyword["ptr"] = PtrKeyword;
	keyword["word"] = WordKey;
	keyword["double word"] = DwKey;
	keyword["byte"] = ByteKey;
	keyword["macro"] = MacroKeyword;
	keyword["end"] = EndKeyword;
	keyword["endm"] = EndmKeyword;
	keyword["ends"] = EndsKeyword;
	keyword["ax"] = Register16;
	keyword["bx'"] = Register16;
	keyword["cx"] = Register16;
	keyword["dx"] = Register16;
	keyword["si"] = Register16;
	keyword["di"] = Register16;
	keyword["ah"] = Register8;
	keyword["al"] = Register8;
	keyword["bh"] = Register8;
	keyword["bl"] = Register8;
	keyword["ch"] = Register8;
	keyword["cl"] = Register8;
	keyword["dh"] = Register8;
	keyword["dl"] = Register8;
	keyword["*"] = Symbol;
	keyword["+"] = Symbol;
	keyword[":"] = Symbol;
	keyword["["] = Symbol;
	keyword["]"] = Symbol;
	keyword["-"] = Symbol;
	keyword[","] = Symbol;
	keyword["="] = Symbol;
}
void output()
{
	cout << "Slovar` for Pidors" << endl;
	for (int i =0;i<vector_of_token.size();i++)
	{
		for (int j = 0; j<vector_of_token[i].size(); j++)
		{
			cout << vector_of_token[i][j].token << " : " << TokenTypeToString(vector_of_token[i][j].type) <<" || ";
		}
		cout << endl;
	}

}

regex hexnumber("[0-9 a-f A-F]+h");
regex binnumber("[0-1]+b");
regex decnumber("[0-9]+d?");
regex text("^\"[A-Z a-z 0-9]+\"$");

TokenType numcheck (string number)
{
	
	if (regex_match(number, hexnumber))
	{
		return HexNumber;
	}
	else if (regex_match(number, binnumber))
	{
		return BinNumber;
	}
	else if (regex_match(number, decnumber))
	{
		return DecNumber;
	}
	else if (regex_match(number, text))
	{
		return Text;
	}
	else
	{
		return Unknown;
	}
}

string trimString(string t)
{
	if (t.length() == 0)
		return "";

	int start = 0, end = t.length() - 1;
	while (t[start] == ' ' || t[start] == '\n' || t[start] == '\t') 
	{
		start++;
		if (start > t.length())
			return "";
	}
	while (t[end] == ' ' || t[end] == '\n' || t[end] == '\t') {
		end--;
		if (end <= 0)
			return "";
	}

	return t.substr(start, end - start + 1);
}

void creatpidshgihlhuy(string t, int line)
{
	string trimmed = trimString(t);//check on " " and tab
	if (trimmed != "")
	{
		itr = keyword.find(trimmed);
		if (itr != keyword.end())
		{
			end_token topush;
			topush.token = trimmed;
			topush.type = itr->second;
			vector_of_token[line].push_back(topush);

		}
		else 
		{
			TokenType check;
			end_token topush;
			check = numcheck(trimmed);
			if (check == Unknown)
			{
				check = Identifier;
			}
			topush.token = trimmed;
			topush.type = check;
			vector_of_token[line].push_back(topush);

		}
		cout << trimmed << "\n";
	}
}

void delimeter()
{	
	bool textflag = false; //0 if no text constant
	int size = 0;
	string token;
	int line = 0;

	FILE* f = fopen("test.txt", "r");
	fseek(f,0, SEEK_END);
	size = ftell(f);	
	fseek(f, 0, SEEK_SET);

	char* buffer = new char[size + 1];
	buffer[size] = 0;
	fread(buffer, size, 1, f);
	vector_of_token.push_back(vector<end_token>());// creating new 0 string for vector (gorizontal)
	for (int i = 0; i < size; i++)
	{
		if (buffer[i] == '\"')
		{
			textflag = !textflag;
			if (textflag)
			{
				creatpidshgihlhuy(token, line);
				token = "";
			}
		}

		if (!textflag)
		{
			if (contains(buffer[i]))
			{
				creatpidshgihlhuy(token, line);
				string temp;
				temp = buffer[i];
				creatpidshgihlhuy(temp, line);
				token = "";
			}
			else
			{
				token += tolower(buffer[i]);
			}
			if (buffer[i] == '\n')
			{
				line++;
				vector_of_token.push_back(vector<end_token>());
			}
		}
		else
		{
			token += buffer[i];
		}
	}
	creatpidshgihlhuy(token,line);
}

bool contains(char s)
{
	return s =='\n'|| s =='\t'|| s == '*' || s == ',' || s == '[' || s == ']' || s == '-' || s == '+' || s == '=' || s == ':' ||s == ' ';
}

string TokenTypeToString(TokenType sex)
{
	switch(sex)
	{
		case DbDirective:
			return "DbDirective";
		case DwDirective:
			return "DwDirective";
		case DdDirective:
			return "DdDirective";
		case Instruction:
			return "Instruction";
		case SegmentKeyword:
			return "SegmentKeyword";
		case MacroKeyword:
			return "MacroKeyword";
		case PtrKeyword:
			return "PtrKeyWord";
		case WordKey:
			return "WordKey";
		case DwKey:
			return "DwKey";
		case ByteKey:
			return "ByteKey";
		case  EndKeyword:
			return " EndKeyword";
		case EndmKeyword:
			return "EndmKeyword";
		case EndsKeyword:
			return "EndsKeyword";
		case Register16:
			return "Register16";
		case Register8:
			return "Register8";
		case Symbol:
			return "Symbol";
		case Identifier:
			return "Identifier";
		case HexNumber:
			return "HexNumber";
		case DecNumber:
			return "DecNumber";
		case BinNumber:
			return "BinNumber";
		case Text:
			return "Text";
		case UserSegment:
			return "UserSegment";
		case SegmentRegister:
			return "SegmentRegister";
		case Label:
			return "Label";
		default:
			return "Unknown";
	}
}