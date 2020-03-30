#include"TokenType.h"
#include"Delimeter.h"
#include"Lexem.h"
#include <cassert>
#include<fstream>

struct end_token //keep information about tokens 
{
	string token;
	TokenType type;
	int token_len;
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
			topush.token_len = 1;
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
			topush.token_len = 1;
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




//LEXEM
TokenType Instruction_Type[] =
{
Instruction,
DbDirective,
DwDirective,
DdDirective,
SegmentKeyword,
MacroKeyword,
EndKeyword,
EndmKeyword,
MacroName,
EndsKeyword };

//int line = 0;


struct Lexem
{
	int instr_index = 0; //instruction index
	int operand_indexes[50];
	int operand_length[50];
	bool has_label = false;
	bool has_name = false;
	bool has_operands = false;
	bool has_instruction = false;
	bool has_macro = false;
	int offset = 0;
	int number_of_operands = 0;
	//vector<end_token>tokens;
};
struct macro_op
{

	string var_name;
	bool has_parameters = false;
	string macro_name;
	string regist;
	TokenType regist_type;
};
struct macro_lines
{
	int line;
	int endm_line;
};
vector<macro_lines>macro_l;
vector<macro_op>macro;
vector<Lexem> lexems;


void create_space_vector()
{
	for (int i = 0; i < vector_of_token.size(); i++)
	{
		lexems.push_back(Lexem());
		macro.push_back(macro_op());
	}
}
void Lexem_create(int line) // checks all cases of complex tokens
{
	if (vector_of_token[line].size() == 2 && vector_of_token[line][0].type == Identifier && (vector_of_token[line][1].type == SegmentKeyword || vector_of_token[line][1].type == EndsKeyword))
	{
		vector_of_token[line][0].type = UserSegment;
		//lexems[line].tokens[line].type = UserSegment;
		lexems[line].has_name = true;
		//needs to add a reiteration check of the segment names

	}
	if (vector_of_token[line].size() == 2 && vector_of_token[line][0].type == Identifier && vector_of_token[line][1].token == ":")
	{
		vector_of_token[line][0].type = Label;
		//lexems[line].tokens[line].type = Label;
		lexems[line].has_label = true;

	}
	if (vector_of_token[line].size() >= 2 && vector_of_token[line][0].type == Identifier && vector_of_token[line][1].type == MacroKeyword)
	{
		macro_lines sh;
		vector_of_token[line][0].type = MacroName;
		macro[line].macro_name = vector_of_token[line][0].token;
		sh.line = line;
		vector<end_token>::iterator itr;
		int iter = 0;
		
		while (vector_of_token[line+iter][0].type != EndmKeyword)
		{

			for (itr = vector_of_token[line+iter].begin(); itr != vector_of_token[line+iter].end(); itr++)
			{
				if (vector_of_token[line+iter][0].type == EndmKeyword)
				{
					sh.endm_line = line + iter;
					break;
				}
				
			}
			iter++;
		}
		macro_l.push_back(sh);
				
		if (vector_of_token[line].size() > 2 && vector_of_token[line][2].type == Identifier)
		{
			macro[line].has_parameters = true;
			macro[line].var_name = vector_of_token[line][2].token;
		}
		//lexems[line].tokens[line].type = Label;
		lexems[line].has_label = true;
		lexems[line].has_macro = true;

	}
	if (vector_of_token[line].size() <= 2 && vector_of_token[line][0].type == Identifier || (vector_of_token[line][1].type == Register16 || vector_of_token[line][1].type == Register8))
	{

		vector_of_token[line][0].type = MacroName;
		//lexems[line].tokens[line].type = Label;0
		lexems[line].has_instruction = true;
	}
	if (vector_of_token[line].size() == 2 && vector_of_token[line][0].token == "jl" && vector_of_token[line][1].type == Label)
	{
		vector_of_token[line][1].type = Label;
		//lexems[line].tokens[line].type = Label;
	}
	has_(line);
}



void has_(int line)
{
	//has label
	if (lexems[line].has_label)
	{
		//lexems[line].has_name = true;
		lexems[line].offset += 2;
		if (vector_of_token[line].size() <= lexems[line].offset)
		{
			int counter=0;
			lexems[line].has_label = true;
			if (vector_of_token[line][0].type == MacroName && (vector_of_token[line][1].type == Register16 || vector_of_token[line][1].type == Register8))  // labels check for MACRO instruction (macro call)
			{	
				lexems[line].has_label = false;
				lexems[line].has_name = false;
				lexems[line].has_instruction = true;
				lexems[line].instr_index = 0;
				if(macro[line].has_parameters == true)
				lexems[line].operand_indexes[1] = 1;
				lexems[line].operand_length[1] = 1;

				macro[line].regist = vector_of_token[line][1].token;//found scale like WITHPAR(macro name) cx(parameter register)
				macro[line].regist_type = vector_of_token[line][1].type;//remembered type of register
				vector<end_token>::iterator itr;
				vector<Lexem>::iterator itra;
				end_token h;
				int it = 0;
				
				if (vector_of_token[macro_l[counter].line][0].token == macro[macro_l[counter].line].macro_name)
				{
					for (int i = macro_l[counter].line + 1; i <= macro_l[counter].endm_line; i++)
					{
						itr = vector_of_token[i].begin();
						itra = lexems.begin()+i;
						for (itr; itr < vector_of_token[i].end(); itr++)
						{
							h.token = vector_of_token[i][it].token;
							if (h.token == macro[i].regist)
							{
								h.token = macro[i].regist;
								h.type = macro[i].regist_type;
								vector_of_token[line].insert(itr, h);
								lexems.insert(itra, lexems[i]);
								continue;
							}
							h.type = vector_of_token[i][it].type;
							vector_of_token[line].insert(itr, h);
							lexems.insert(itra, lexems[i]);
							it++;
						}
						it = 0;
					}
				}
				
				
				lexems[line].has_operands = true;
				lexems[line].operand_indexes[1] = 1;
				lexems[line].operand_length[1] = 1;
				return;
			}
			else if (vector_of_token[line][1].type == MacroKeyword)// check for MACRO instruction (macro init with no parameters)
			{
				lexems[line].has_instruction = true;
				lexems[line].instr_index = 1;
				return;
			}
		
			else
			{
				lexems[line].has_instruction = false;
				lexems[line].has_operands = false;
			}
			return;
		}
	}
		 if (vector_of_token[line][0].type == MacroName && vector_of_token[line][1].type == MacroKeyword && vector_of_token[line][2].type == Identifier)//// check for MACRO instruction (macro init with 1 parameters)
		 {
			 macro[line].has_parameters = true;
			lexems[line].has_instruction = true;
			lexems[line].instr_index = 1;
			lexems[line].has_operands = true;
			lexems[line].operand_indexes[1] = 2;
			lexems[line].operand_length[1] = 1;
			return;
		 }
		 

	int flag = 0;
	//has name
	if (vector_of_token[line][0].type == UserSegment && vector_of_token[line][1].type == EndsKeyword)
	{
		lexems[line].has_instruction = true;
		lexems[line].instr_index = 1;
		lexems[line].has_operands = false;
		return;

	}
	if (vector_of_token[line][0].type == Identifier || vector_of_token[line][0].type == UserSegment)
	{
		lexems[line].has_name = true;
		lexems[line].offset += 1;
	
	}
	// instruction check
	for (int i = 0; i < 8; i++)
	{
		if (vector_of_token[line][lexems[line].offset].type == Instruction_Type[i])
		{
			lexems[line].has_instruction = true;
			flag = 1;
			break;
		}
	}
	//flag for instruction check
		if (flag == 0)
		{
			lexems[line].has_instruction = false;
			lexems[line].has_operands = false;
			return;
		}
		//if we have an instruction
		else 
		{
			lexems[line].instr_index = lexems[line].offset;
			
			if (vector_of_token[line].size() <= lexems[line].offset+1)
			{
				lexems[line].has_operands = false;
				return;
			}
			//if we have operands
			else
			{
					lexems[line].offset += 1;
					lexems[line].has_operands = true;
					
					lexems[line].operand_indexes[1] = (lexems[line].offset);
					lexems[line].operand_length[line] = 0;

					vector<end_token>::iterator itr;
					int i = (lexems[line].offset);

					for (int j = i; j < vector_of_token[line].size(); j++)
					{
						if (vector_of_token[line][j].type == Symbol && vector_of_token[line][j].token == ",")
						{
							lexems[line].operand_indexes[2] = lexems[line].operand_length[lexems[line].number_of_operands+1] + lexems[line].number_of_operands + 1 + i;
							lexems[line].number_of_operands += 1;
						}
						else
						{
							lexems[line].operand_length[lexems[line].number_of_operands+1]+=1;
						}
					}
				
					return;
			}
		}
}

void cycle_creator() // call cycle to create lexems for each lines of tokens
{
	emptyCheck();//delete empty str
	create_space_vector(); //creat space for vector of lexems 
	
	for (int i = 0; i < vector_of_token.size(); i++)
	{
		Lexem_create(i);
		
	}
	output();
}

void output()
{
	string mas;
	char ch = '\n';
	ifstream f("test.txt", ios::in);
	if (!f)cout << "cannot open file";

	for (int i = 0; i < vector_of_token.size(); i++)
	{
		getline(f, mas);
		if (mas == "\r" ||  mas == "\t" || mas == "\0")
		{
			getline(f, mas);
		}
		cout << "Source string #:" << i+1 << " = " << mas << endl;

		cout << "==================================================" << endl;
		cout << "#line" << "\t" << "#token" << "   " << "Token" << "\t" << "Type of Token" << "\t\t||" << endl;
		cout << "==================================================" << endl;
		cout << i+1 << "::" << "";
		for (int j = 0; j < vector_of_token[i].size(); j++)
		{
			cout << "\t" << j << "\t" << vector_of_token[i][j].token << "\t: (" << TokenTypeToString(vector_of_token[i][j].type) << ")" << endl;
			
		}
		cout << "--------------------------------------------------" << "\n";
		cout << "TRANSCRIPT:(name field | mnemo | operand1| operand2|)"<<"\n\n";
		cout << "LEXEM:  ";
		if (lexems[i].has_name || lexems[i].has_label) cout << "|" << "0" << "| ";
		 if(!lexems[i].has_name && !lexems[i].has_label) cout << "|" << "---" << "|";
		 if (lexems[i].has_instruction) cout << " " << lexems[i].instr_index << " " << "1"<< " | ";
		 if(!lexems[i].has_instruction) cout<< "|" << "---" << "|";
		 if (lexems[i].has_operands)
		{
			for (int k = 0; k <= lexems[i].number_of_operands; k++)
			{
				cout << "  " << lexems[i].operand_indexes[1+k] << " " << lexems[i].operand_length[1+k] << " | ";
			}
		}
		 else cout << "|" << "---" << "|"<<" |"<<"---"<<"|";
			cout << endl;
			cout << "\n\n\n";
	}
	
	f.close();
}