#include "TokenType.h"
#include"Lexem.h"
#include"Token_and_delimeter.cpp"
#include <list>

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
EndsKeyword };

//int line = 0;


struct Lexem
{
	int instr_index = 0; //instruction index
	int operand_indexes[10];
	int operand_length[10];
	bool has_label = 0;
	bool has_name = 0;
	bool has_operands = 0;
	bool has_instruction = 0;
	int offset = 0;
	vector<end_token>tokens;
};

vector<Lexem> lexems;


void Lexem_create(int line)
{
	if (vector_of_token[line].size()==2 && vector_of_token[line][0].type == Identifier && vector_of_token[line][1].type ==SegmentKeyword)
	{
		vector_of_token[line][0].type = UserSegment;
		lexems[line].tokens[line].type = UserSegment;

		//needs to add a reiteration check of the segment names
		
	}
	if (vector_of_token[line].size >=2 && vector_of_token[line][0].type==Identifier && vector_of_token[line][1].token!=":")
	{

	}
}


//WARNING:
//NEEDS SOME TEST
//AND IMPROVMENTS
//IN THIS PART OF CODE "garachechnij bred (c)Uzorin"
//CAN BE FOUND
//CLEAN YOUR EYES AFTER LOKKING AT THIS CODE!
//PLEASE BE PACIENT AND KEEP CALM! :)
void has_(int line)
{
	//has label
	if (lexems[line].has_label)
	{
		lexems[line].has_name = true;
		lexems[line].offset += 2;
	}
	if (lexems[line].tokens.size() <= lexems[line].offset)
	{
		lexems[line].has_label = true;
		lexems[line].has_instruction = false;
	}
	
	int flag = 0;
	//has name
	if (vector_of_token[line][0].type == Identifier || vector_of_token[line][0].type == UserSegment)
	{
		lexems[line].has_name = true;
		for (int i = 0; i < 8; i++)
		{
			if (vector_of_token[line][1].type == Instruction_Type[i])
			{
				lexems[line].has_instruction = true;
				flag = 1;
				break;
			}
		}
		if (flag == 0)
		{
			lexems[line].has_instruction = false;
			return;
		}
		else
		{
			lexems[line].instr_index = lexems[line].offset;
			lexems[line].offset += 1;
			if (lexems[line].tokens.size() <= lexems[line].offset)
			{
				lexems[line].has_operands = false;
				return;
			}
			else
			{
				int number_of_operands = 0;
				lexems[line].operand_indexes[line]=(lexems[line].offset);
				lexems[line].operand_length[line] = 0;

				vector<end_token>::iterator itr;
				int i = (lexems[line].offset);
				
				for (itr = lexems[line].tokens.begin()+i;itr!=lexems[line].tokens.end();itr++)
				{
					if (lexems[line].tokens[line].type == Symbol && lexems[line].tokens[line].token == ",")
					{
						lexems[line].operand_indexes[line] = lexems[line].operand_length[number_of_operands] + number_of_operands + 1 + i;
						number_of_operands += 1;
					}
					else
					{
						lexems[line].operand_length[number_of_operands]++;
					}
				}
			}
		}
	}
}