#include "Assembly.h"
#include "Delimiter.h"

vector<UserName> userNames;

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
      for(int t = 0; t < vectorOfTokens[l].size(); t++)
         cout << vectorOfTokens[l][t].token << " ";
      cout << "\n";
   }
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

      const vector<end_token> &vector = vectorOfTokens[l];

      for(int op = 0; op < lexems[l].numberOfOperands; op++)
      {
         // Vector of tokens in current operand
         int index = lexems[l].operandLengths[op + 1];
         int length = lexems[l].operandLengths[op + 1];

         //std::vector<end_token*> operand;
         //for(int i = index; i < index + length; i++)
         //   operand.push_back(&vectorOfTokens[l][i]);

         //std::cout << vectorOfTokens[l][index].token;
      }
   }
}
