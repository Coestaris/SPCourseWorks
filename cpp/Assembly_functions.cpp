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
		if(lexems[l].hasInstruction && lexems[l].hasOperands)
			for (int t = 0; t <= lexems[l].numberOfOperands; t++)
				cout << lexems[l].operandTypes[t] << " ";

	 
      for(int t = 0; t < vectorOfTokens[l].size(); t++)
         cout << vectorOfTokens[l][t].token << " ";
      cout << "\n";
   }
}

UserName* getUserName(UserNameType type, int line)
{
	for (auto& a : userNames)
		if (a.type == type && a.begin >= line && a.end <= line)
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

			 // mem
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

				 if(lbl->begin < l)
					lexems[l].operandTypes[op] = OT_LabelFwd;
				 else
					 lexems[l].operandTypes[op] = OT_LabelBack;
			 }
		 }
      }
   }
}
