#include "Assembly.h"
#include "Delimiter.h"

// Some macro for analyzeOperandTypes.
// Check operand to be a specific type
#define check(pos, should, msg)                  \
	if(operand[pos]->type != should)		     \
	{											 \
		lexems[l].SetError(msg, *operand[pos]);  \
		continue;						         \
	}
#define check2(pos, should1, should2, msg)								\
	if(operand[pos]->type != should1 && operand[pos]->type != should2)	\
	{																	\
		lexems[l].SetError(msg, *operand[pos]);							\
		continue;														\
	}
#define check_c(pos, c, msg)										\
	if(operand[pos]->type != Symbol || operand[pos]->token[0] != c) \
	{																\
		lexems[l].SetError(msg, *operand[pos]);						\
		continue;													\
	}

vector<UserName> userNames;

// Creates segments and fills their parameters
void analyzeSegments()
{
   for(int l = 0; l < vectorOfTokens.size(); l++)
   {
      if(lexems[l].has_error)
         continue;

      const vector<end_token>& vector = vectorOfTokens[l];
      if(vector.size() == 2 && vector[0].type == UserSegment && vector[1].type == SegmentKeyword)
      {
         auto segment = getUserName(NT_Segment, vector[0].token);
         if(segment)
         {
            lexems[l].SetError("Segment with that name already exists", vector[0]);
            continue;
         }

         UserName name;
         name.begin = l;
         name.type = NT_Segment;
         name.token = vector[0];

         userNames.push_back(name);
      }
      else if(vector.size() == 2 && vector[0].type == UserSegment && vector[1].type == EndsKeyword)
      {
         auto segment = getUserName(NT_Segment, vector[0].token);
         if(!segment)
         {
            lexems[l].SetError("Unable to close nonexisting segment", vector[0]);
            continue;
         }

         segment->end = l;
      }
   }
}

// Creates variables and labels, and fills their parametrs
void analyzeVariablesAndLabels()
{
   for(int l = 0; l < vectorOfTokens.size(); l++)
   {
      if (lexems[l].has_error)
         continue;

      const vector<end_token>& vector = vectorOfTokens[l];
      if(vector.size() == 2 && vector[0].type == Label && vector[1].type == Symbol && vector[1].token == ":")
      {
         auto label = getUserName(NT_Label, vector[0].token);
         if(label)
         {
            lexems[l].SetError("Label with that name already exists", vector[0]);
            continue;
         }

         UserName name;
         name.begin = l;
         name.end = l;
         name.type = NT_Label;
         name.token = vector[0];

         userNames.push_back(name);
      }
      else if(vector.size() == 3 && vector[0].type == Identifier &&
         (vector[1].type == DbDirective || vector[1].type == DwDirective || vector[1].type == DdDirective))
      {
         auto var = getUserName(NT_Var, vector[0].token);
         if(var)
         {
            lexems[l].SetError("Variable with that name already exists", vector[0]);
            continue;
         }

         UserName name;
         name.begin = l;
         name.end = l;
         name.type = NT_Var;
         name.token = vector[0];

         userNames.push_back(name);
      }
   }
}

void printLexemeList()
{
   for(int l = 0; l < vectorOfTokens.size(); l++)
   {
	//if(lexems[l].hasInstruction && lexems[l].hasOperands)
	//	for (int t = 0; t <= lexems[l].numberOfOperands; t++)
	//		cout << lexems[l].operandTypes[t] << " ";
	   cout << lexems[l].offset << " :::  ";
	 
      for(int t = 0; t < vectorOfTokens[l].size(); t++)
         cout << vectorOfTokens[l][t].token << " ";
      cout << "\n";
   }
}

UserName* getUserName(UserNameType type, int line)
{
	for (auto& a : userNames)
		if (a.type == type && a.begin <= line && a.end >= line)
			return &a;

	return nullptr;
}

UserName* getUserName(UserNameType type, const string& name)
{
   for(auto& a : userNames)
      if(a.type == type && a.token.token == name)
         return &a;

   return nullptr;
}

void outputErrors()
{
   int e = 0;
   for(int l = 0; l < vectorOfTokens.size(); l++)
   {
      if(lexems[l].has_error)
      {
         cout << "Error: \"" + lexems[l].error << "\" at line " << l << " at lexeme \"" << lexems[l].error_token.token << "\"\n";
         e++;
      }
   }
   cout << "\n";
   cout << e << " errors.";
}

void analyzeOperandTypes()
{
   for(int l = 0; l < vectorOfTokens.size(); l++)
   {
      if (lexems[l].has_error)
         continue;

      if(!lexems[l].hasInstruction || !lexems[l].hasOperands)
         continue;

	  // Skip macro declarations, they are strange...
	  if (lexems[l].hasMacro)
		  continue;

      const vector<end_token> &vector = vectorOfTokens[l];

      for(int op = 0; op < lexems[l].numberOfOperands + 1; op++)
      {
         // Vector of tokens in current operand
         int index = lexems[l].operandIndices[op + 1];
         int length = lexems[l].operandLengths[op + 1];

         std::vector<end_token*> operand;
         for(int i = index; i < index + length; i++)
            operand.push_back(&vectorOfTokens[l][i]);
		 
		 if (operand.size() != 1)
		 {
			 // memory location
			 // min len = 5, max = 9
			 if (operand.size() < 5)
			 {
				 lexems[l].SetError("Operand is too short", *operand[0]);
				 continue;
			 }
			 if (operand.size() > 9)
			 {
				 lexems[l].SetError("Operand is too long", *operand[0]);
				 continue;
			 }

			 int position = 0;
			 if (operand[position]->type == ByteKey)
			 {
				 lexems[l].operandTypes[op] = OT_Memory8;
				 check(position + 1, PtrKeyword, "Expected PTR");
				 position += 2;
			 }
			 else if (operand[position]->type == WordKey)
			 {
				 lexems[l].operandTypes[op] = OT_Memory16;
				 check(position + 1, PtrKeyword, "Expected PTR");
				 position += 2;
			 }
			 else
			 {
				 lexems[l].operandTypes[op] = OT_Memory;
			 }

			 if (operand[position]->type == Identifier || operand[position]->type == SegmentRegister)
			 {
				 lexems[l].segment_prefix = *operand[position];
				 lexems[l].has_segment_prefix = true;
				 check_c(position + 1, ':', "Expected ':'");
				 position += 2;
			 }

			 if (operand.size() - position != 5)
			 {
				 lexems[l].SetError("Operand has wrong structure", *operand[0]);
				 continue;
			 }
			 
			 check_c(position + 0, '[', "Expected '['");
			 check2 (position + 1, Register8, Register16, "Expected register");
			 check_c(position + 2, '+', "Expected '+'");
			 check2 (position + 3, Register8, Register16, "Expected register");
			 check_c(position + 4, ']', "Expected ']'");

			 if (operand[position + 1]->type != operand[position + 3]->type)
			 {
				 lexems[l].SetError("Registers must have equal size", *operand[position + 1]);
				 continue;
			 }
		 }
		 else
		 {
			 // constant register or label
			 end_token* t = operand[0];

			 if (t->type == Register8)
				 lexems[l].operandTypes[op] = OT_Register8;
			 else if (t->type == Register16)
				 lexems[l].operandTypes[op] = OT_Register16;
			 else if (t->type == BinNumber || t->type == DecNumber || t->type == HexNumber)
			 {
				 uint64_t number = 0;
				 if (t->type == BinNumber)
				 {
					 string toConvert = t->token.substr(0, t->token.size() - 1);
					 number = stoul(toConvert, nullptr, 2);
				 }
				 else if (t->type == HexNumber)
				 {
					 string toConvert = t->token.substr(0, t->token.size() - 1);
					 number = stoul(toConvert, nullptr, 16);
				 }
				 else
				 {
					 number = stoul(t->token);
				 }
				 
				 if (number < 0xFF)
					 lexems[l].operandTypes[op] = OT_Const8;
				 else 
					 lexems[l].operandTypes[op] = OT_Const16;
			 }
			 else
			 {
				 // I dunno what to with text here...
				 if (t->type == Text)
					 continue;

				 if (t->type != Identifier)
				 {
					 lexems[l].SetError("Expected label", *t);
					 continue;
				 }

				 UserName* lbl = getUserName(NT_Label, t->token);
				 if (lbl == nullptr)
				 {
					 // Maybe, its not label, but macro parameter... "variable"
					 // If line stand out of segments, it mean current line is insede macro
					 UserName* seg = getUserName(NT_Segment, l);
					 if (seg == nullptr)
						 continue;

					 lexems[l].SetError("Unkwown label", *t);
					 continue;
				 }

				 if(lbl->begin > l)
					lexems[l].operandTypes[op] = OT_LabelFwd;
				 else
					 lexems[l].operandTypes[op] = OT_LabelBack;
			 }
		 }
      }
   }
}

bool check_parameters(Lexem& l, const end_token& token, initializer_list<operandType> list)
{
	// Check operand count
	if(!l.hasOperands)
	{
		if (list.size() != 0)
		{
			l.SetError("Wrong parameter count", token);
			return false;
		}
	}
	else
	{
		if (l.numberOfOperands + 1 != list.size())
		{
			l.SetError("Wrong parameter count", token);
			return false;
		}
	}

	int i = 0;
	bool wrong = false;
	for (auto param : list)
	{
		wrong = true;
		if ((param == OT_Const8)    && (l.operandTypes[i] != OT_Const8    && l.operandTypes[i] != OT_Const16)) { break; }
		if ((param == OT_Register8) && (l.operandTypes[i] != OT_Register8 && l.operandTypes[i] != OT_Register16)) { break; }
		if ((param == OT_Memory)    && (l.operandTypes[i] != OT_Memory    && l.operandTypes[i] != OT_Memory8 && l.operandTypes[i] != OT_Memory16)) { break; }
		if ((param == OT_LabelBack) && (l.operandTypes[i] != OT_LabelFwd  && l.operandTypes[i] != OT_LabelBack)) { break; }
		
		wrong = false;
		i++;
	}

	if (wrong)
	{
		l.SetError("Wrong parameter type.", token);
		return false;
	}

	return true;
}

void checkInsrtuctionRequirements()
{
	for (int l = 0; l < vectorOfTokens.size(); l++)
	{
		lexems[l].offset = 0;

		if (lexems[l].has_error)
			continue;
	
		if (!lexems[l].hasInstruction)
			continue;

		// Skip instructions in macro
		UserName* seg = getUserName(NT_Segment, l);
		if (seg == nullptr)
			continue;

		const vector<end_token>& vector = vectorOfTokens[l];

		// Skip db, macro and other nonInstruction shit
		const end_token& instruction = vector[lexems[l].instrIndex];
		if (instruction.type != Instruction)
			continue;

		operandType op1 = lexems[l].operandTypes[0];
		operandType op2 = lexems[l].operandTypes[1];

		if (instruction.token == "aas") {
			check_parameters(lexems[l], instruction, {});
		}
		else if (instruction.token == "inc") {
			check_parameters(lexems[l], instruction, { OT_Register8 });
		}
		else if (instruction.token == "neg") {
			check_parameters(lexems[l], instruction, { OT_Memory });
		}
		else if (instruction.token == "bt") 
		{
			if (!check_parameters(lexems[l], instruction, { OT_Register8, OT_Register8 }))
				continue;
			if (op1 != op2) {
				lexems[l].SetError("Operands type mismatch", vector[0]);
			}
		}
		else if (instruction.token == "and") 
		{
			if (!check_parameters(lexems[l], instruction, { OT_Register8, OT_Memory }))
				continue;

			if ((op1 == OT_Register8 && op2 == OT_Memory16) || (op1 == OT_Register16 && op2 == OT_Memory8)) {
				lexems[l].SetError("Operands type mismatch", vector[0]);
			}
		}
		else if (instruction.token == "cmp")
		{
			if (!check_parameters(lexems[l], instruction, { OT_Memory, OT_Register8 }))
				continue;
			
			if ((op1 == OT_Memory8 && op2 == OT_Register16) || (op1 == OT_Memory16 && op2 == OT_Register8)) {
				lexems[l].SetError("Operands type mismatch", vector[0]);
			}
		}
		else if (instruction.token == "mov") {
			if (!check_parameters(lexems[l], instruction, { OT_Register8, OT_Const8 }))
				continue;

			if ((op1 == OT_Register8 && op2 == OT_Memory16)) {
				lexems[l].SetError("Operands type mismatch", vector[0]);
			}
		}
		else if (instruction.token == "or") {
			if (!check_parameters(lexems[l], instruction, { OT_Memory, OT_Const8 }))
				continue;

			if ((op1 == OT_Register8 && op2 != OT_Const8)) {
				lexems[l].SetError("Operands type mismatch", vector[0]);
			}
		}
		else if (instruction.token == "jl") {
			check_parameters(lexems[l], instruction, { OT_LabelBack });
		}
		else
		{
			lexems[l].SetError("Unkown instruction", instruction);
		}
	}
}

// 3F  AAS

// FE /0 INC r/m8 
// 40+rw INC r16

// F6 /3 NEG r/m8
// F7 /3 NEG r/m16

// 0F A3 BT r/m16, r16 

// 22 /r AND r8, r/m8 
// 23 /r AND r16, r/m16 

// 38 /r CMP r/m8, r8
// 39 /r CMP r/m16, r16

// B0+rb MOV reg8, imm8
// B8+rw MOV reg16, imm16

// 80 /1 ib OR r/m8, imm8
// 83 /1 ib OR r/m16, imm8
// 81 /1 iw OR r/m16, imm16

// 7C cb JL rel8 
// OF 8C cw JL rel16 
void calculateSize()
{
	for (int l = 0; l < vectorOfTokens.size(); l++)
	{
		if (lexems[l].has_error)
			continue;

		if (!lexems[l].hasInstruction)
			continue;

		UserName* seg = getUserName(NT_Segment, l);
		if (seg == nullptr)
			continue;

		const vector<end_token>& vector = vectorOfTokens[l];
		int size = 0;

		operandType op1 = lexems[l].operandTypes[0];
		operandType op2 = lexems[l].operandTypes[1];

		// Skip db, macro and other nonInstruction shit
		const end_token& instruction = vector[lexems[l].instrIndex];
		if (instruction.type != Instruction)
		{
			//We need DB, DW and DD to calculate its size
		}
		else
		{
			if (instruction.token == "aas") {
				size = 1; // OPCODE
			}
			else if (instruction.token == "inc") {
				if (op1 == OT_Register8)
					size = 2; // OPCODE + MODRM
				else
					size = 1; // Just Opcode. Destination stored in opcode
			}
			else if (instruction.token == "neg") {
				size = 3; // OPCODE + MORM + SIB
			}
			else if (instruction.token == "bt") {
				size = 3; // EXP PREF + MODRM + SIB
			}
			else if (instruction.token == "and") {
				size = 3; // OPCODE + MORM + SIB
			}
			else if (instruction.token == "cmp") {
				size = 3; // OPCODE + MORM + SIB
			}
			else if (instruction.token == "mov") {
				size = 3; // OPCODE (With packed register) + CONST16
			}
			else if (instruction.token == "or") {
				size = 3; // OPCODE + MODRM + SIB

				// CONST SIZE
				if (op2 == OT_Const8)
					size += 1;
				else
					size += 2;
			}
			else if (instruction.token == "jl") {
				if (op1 == OT_LabelBack)
					size = 2; // OPCODE + OFFSET
				else
					size = 6; // OPCODE + OFFSET + 
					// + (possible 4 bytes for EXP PREFIX and far jump. Thats what TASM and MASM do, Idk...)
			}

			if (lexems[l].has_segment_prefix)
				size += 1;
		}

		lexems[l].offset = size;
		seg->offset += size;
	}
}
