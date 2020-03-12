

#include"TokenType.h"
#include"Delimeter.h"
#include <cassert>

struct end_token //keep information about tokens 
{
	string token;
	TokenType type;

};
vector<vector<end_token>> vector_of_token; // two-dimensional vector of vectors to sort tokens by the lines of the input file

unordered_map<string, TokenType >keyword;
unordered_map<string, TokenType>::iterator itr;




//TOKEN

void input()//creating dictionary
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
	keyword["bx"] = Register16;
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
	emptyCheck();
	for (int i =0;i<vector_of_token.size();i++)
	{
		cout << "==================================================" << endl;
		cout << "#line"<<"\t"<<"#token" << "   " << "Token"<<"\t" << "Type of Token"<<"\t\t||"<< endl;
		cout << "==================================================" << endl;
		cout<<i << "::"<<"";
		for (int j = 0; j<vector_of_token[i].size(); j++)
		{
			cout<<"\t"<<j+1<<"\t" <<vector_of_token[i][j].token << "\t: (" << TokenTypeToString(vector_of_token[i][j].type) <<")"<<endl;
		}
		cout<<"--------------------------------------------------" <<"\n\n"<< endl;
	}

}


//regular expresions for finding types of numeric constants
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



string TokenTypeToString(TokenType tochange)
{
	switch(tochange)
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

//DELIMETER
string trimString(string t)
{
	if (t.length() == 0) return"";

	int start = 0, end = t.length() - 1;
	while (isEmptySpace(t[start]))
	{
		start++;
		if (start > t.length()) return ""; 
	}
	while (isEmptySpace(t[end])) {
		end--;
		if (end <= 0) return "";
	}

	return t.substr(start, end - start + 1);
}

void vectorfill(string t, int line)
{
	string trimmed = trimString(t); //check on " " and tab
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
		//cout << trimmed << "\n";
	}
}

void delimeter()
{
	bool textflag = false; //false if no text constant
	int size = 0;
	string token;
	int line = 0;

	FILE* f = fopen("test.txt", "rb");
	assert(f); // check if f is NULL

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buffer = new char[size + 1];
	memset(buffer, 0, size + 1);

	fread(buffer, size, 1, f);
	buffer[size] = '\0';

	vector_of_token.push_back(vector<end_token>());// creating new 0 string for vector (gorizontal)
	for (int i = 0; i < size; i++)
	{
		
		if (buffer[i] == '\"')//check if start/end of text constant
		{
			textflag = !textflag; //flag for ignoring delimetering
			if (textflag)
			{
				vectorfill(token, line);
				token = "";
			}
		}

		if (!textflag) //default token delimeter if !text const
		{
			if (isDelimeter(buffer[i]))
			{
				vectorfill(token, line);
				string temp;
				temp = buffer[i];
				vectorfill(temp, line);
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
	vectorfill(token, line);

	fclose(f);

	
}

bool isEmptySpace(int c)
{
	return c == '\r' || c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

bool isDelimeter(char s) //delimeter symbols
{
	return isEmptySpace(s) || s == '*' || s == ',' || s == '[' || s == ']' || s == '-' || s == '+' || s == '=' || s == ':';
}

void emptyCheck() //delete empty strings
{
		for (int i = vector_of_token.size() - 1; i >= 0; i--)
		{
			if (vector_of_token[i].size() == 0)
				vector_of_token.erase(vector_of_token.begin() + i);
		}
}