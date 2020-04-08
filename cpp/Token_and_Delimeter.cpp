#include"TokenType.h"
#include"Delimiter.h"
#include"Lexem.h"
#include <cassert>
#include<fstream>
#include <map>

// two-dimensional vector of vectors to sort tokens by the lines of the input file
vector<vector<end_token>> vectorOfTokens;
vector<Lexem> lexems;

static unordered_map<string, TokenType> keyword;
static unordered_map<string, TokenType>::iterator itr;

// Types to determine instructions
static TokenType instructionType[] =
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
   EndsKeyword
};

// Macro stuff
static vector<struct macro> macro;
static struct macro listeningMacro;

//regular expresions for finding types of numeric constants
static regex hexnumber("[0-9 a-f A-F]+h");
static regex binnumber("[0-1]+b");
static regex decnumber("[0-9]+d?");
static regex text("^\"[A-Z a-z 0-9]+\"$");


// Indicates that char is non-printable empty space
static bool isEmptySpace(int c)
{
   return c == '\r' || c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

// Returns true for characters that will be used for splitting source
static bool isDelimiter(char s)
{
   return isEmptySpace(s) || s == '*' || s == ',' || s == '[' || s == ']' || s == '-' || s == '+' || s == '=' ||
          s == ':';
}


// Try to determine token type by regular expressions
static TokenType numCheck(const string& number)
{
   if (regex_match(number, hexnumber))
      return HexNumber;
   else if (regex_match(number, binnumber))
      return BinNumber;
   else if (regex_match(number, decnumber))
      return DecNumber;
   else if (regex_match(number, text))
      return Text;
   else
      return Unknown;
}

// Returns true if line use macro
static struct macro* hasMacro(const vector<end_token>& tokens)
{
   for (const end_token &tk : tokens)
      if (tk.type == MacroName || tk.type == Identifier)
         for (auto & i : macro)
            if (tk.token == i.name.token)
               return &i;

   return nullptr;
}

// Remove whitespaces in string from both sides
static string trimString(const string& t)
{
   if (t.length() == 0) return "";

   int start = 0, end = t.length() - 1;
   while (isEmptySpace(t[start]))
   {
      start++;
      if (start > t.length()) return "";
   }
   while (isEmptySpace(t[end]))
   {
      end--;
      if (end <= 0) return "";
   }

   return t.substr(start, end - start + 1);
}

// Check token and append it to global tokens list
static void appendToken(const string& t, int line)
{
   //check on " " and tab
   string trimmed = trimString(t);
   if (!trimmed.empty())
   {
      itr = keyword.find(trimmed);
      if (itr != keyword.end())
      {
         end_token topush;
         topush.token = trimmed;
         topush.type = itr->second;
         vectorOfTokens[line].push_back(topush);

      }
      else
      {
         TokenType check;
         end_token topush;
         check = numCheck(trimmed);
         if (check == Unknown)
         {
            check = Identifier;
         }
         topush.token = trimmed;
         topush.type = check;
         vectorOfTokens[line].push_back(topush);
      }
   }
}

// Delete empty strings
static void emptyCheck()
{
   for (int i = vectorOfTokens.size() - 1; i >= 0; i--)
   {
      if (vectorOfTokens[i].empty())
         vectorOfTokens.erase(vectorOfTokens.begin() + i);
   }
}

// Allocate lexemes
static void createSpaceVector()
{
   for (int i = 0; i < vectorOfTokens.size(); i++)
   {
      lexems.emplace_back();
   }
}

// Add static values and its types
void setupTokenTypeDict()
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

// Convert tokenType to its string representation
string tokenTypeToString(TokenType type)
{
   switch (type)
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
      case EndKeyword:
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
      case MacroName:
         return "MacroName";
      default:
         return "Unknown";
   }
}

// Read source file and create tokenss
void createTokens(const string& filename)
{
   bool textflag = false; //false if no text constant
   int size = 0;
   string token;
   int line = 0;

#if __linux__ // For linux compatibility
   FILE* f = fopen(("../" + filename).c_str(), "rb");
#else
   FILE* f = fopen(filename.c_str(), "rb");
#endif

   assert(f); // check if f is NULL

   fseek(f, 0, SEEK_END);
   size = ftell(f);
   fseek(f, 0, SEEK_SET);

   char* buffer = new char[size + 1];
   memset(buffer, 0, size + 1);

   fread(buffer, size, 1, f);
   buffer[size] = '\0';

   // creating new 0 string for vector (horizontal)
   vectorOfTokens.emplace_back();
   for (int i = 0; i < size; i++)
   {
      //check if start/end of text constant
      if (buffer[i] == '\"')
      {
         //flag for ignoring delimetering
         textflag = !textflag;

         if (textflag)
         {
            appendToken(token, line);
            token = "";
         }
      }

      //default token createTokens if !text const
      if (!textflag)
      {
         if (isDelimiter(buffer[i]))
         {
            appendToken(token, line);
            string temp;
            temp = buffer[i];
            appendToken(temp, line);
            token = "";
         }
         else
         {
            token += tolower(buffer[i]);
         }
         if (buffer[i] == '\n')
         {
            line++;
            vectorOfTokens.emplace_back();
         }
      }
      else
      {
         token += buffer[i];
      }
   }
   appendToken(token, line);

   delete[] buffer;
   fclose(f);
}

// Checks all cases of complex tokens
void lexemeCreate(int line)
{
   if (vectorOfTokens[line].size() == 2 && vectorOfTokens[line][0].type == Identifier &&
       (vectorOfTokens[line][1].type == SegmentKeyword || vectorOfTokens[line][1].type == EndsKeyword))
   {
      vectorOfTokens[line][0].type = UserSegment;
      //lexems[line].tokens[line].type = UserSegment;
      lexems[line].hasName = true;
      //needs to add a reiteration check of the segment names

   }
   if (vectorOfTokens[line].size() == 2 && vectorOfTokens[line][0].type == Identifier &&
       vectorOfTokens[line][1].token == ":")
   {
      vectorOfTokens[line][0].type = Label;
      //lexems[line].tokens[line].type = Label;
      lexems[line].hasLabel = true;

   }

   // Macro declaration
   if (vectorOfTokens[line].size() >= 2 && vectorOfTokens[line][0].type == Identifier &&
       vectorOfTokens[line][1].type == MacroKeyword)
   {
      struct macro m;
      vectorOfTokens[line][0].type = MacroName;
      m.name = vectorOfTokens[line][0];
      m.start = line;
      m.start = line;

      lexems[line].hasLabel = true;
      lexems[line].hasMacro = true;

      if (vectorOfTokens[line].size() != 2) // has parameters
      {
         for (int i = 2; i < vectorOfTokens[line].size(); i++) // iterate through parameters
            m.parameters.push_back(vectorOfTokens[line][i]);
      }

      listeningMacro = m;
   }
   else if (vectorOfTokens[line].size() == 1 && vectorOfTokens[line][0].type == EndmKeyword)
   {
      // End of the macro
     // lexems[line].hasLabel = true;
      listeningMacro.end = line;
      macro.push_back(listeningMacro);
   }

   determineStructure(line);
}

// Determining tokens structure =3
void determineStructure(int line)
{
   //has label
   if (lexems[line].hasLabel)
   {
      //lexems[line].hasName = true;
      lexems[line].offset += 2;
      if (vectorOfTokens[line].size() <= lexems[line].offset)
      {
         int counter = 0;
         lexems[line].hasLabel = true;
        
         if (vectorOfTokens[line].size() == 2 && vectorOfTokens[line][1].type ==
                                                      MacroKeyword)// check for MACRO instruction (macro init with no parameters)
         {
            lexems[line].hasInstruction = true;
            lexems[line].instrIndex = 1;
            return;
         }

         else
         {
            lexems[line].hasInstruction = false;
            lexems[line].hasOperands = false;
         }
         return;
      }
   }
   if (vectorOfTokens[line].size() == 1 && vectorOfTokens[line][0].type == Identifier)
   {
       vectorOfTokens[line][0].type = MacroName;
       lexems[line].hasLabel = false;
       lexems[line].hasName = false;
       lexems[line].hasInstruction = true;
       lexems[line].instrIndex = 0;
       lexems[line].hasOperands = false;
       return;
   }
   if (vectorOfTokens[line].size() == 2 && vectorOfTokens[line][0].type == Identifier && (vectorOfTokens[line][1].type == Register16 ||
       vectorOfTokens[line][1].type ==
       Register8))  // labels check for MACRO instruction (macro call)
   {
       vectorOfTokens[line][0].type = MacroName;
       lexems[line].hasLabel = false;
       lexems[line].hasName = false;
       lexems[line].hasInstruction = true;
       lexems[line].instrIndex = 0;
       lexems[line].hasOperands = true;
       lexems[line].operandIndices[1] = 1;
       lexems[line].operandLengths[1] = 1;
       return;
   }
   if (vectorOfTokens[line].size() == 3 && vectorOfTokens[line][0].type == MacroName &&
       vectorOfTokens[line][1].type == MacroKeyword &&
       vectorOfTokens[line][2].type == Identifier)//// check for MACRO instruction (macro init with 1 parameters)
   {
     // vectorOfTokens[line][0].type = MacroName;
      lexems[line].hasInstruction = true;
      lexems[line].instrIndex = 1;
      lexems[line].hasOperands = true;
      lexems[line].operandIndices[1] = 2;
      lexems[line].operandLengths[1] = 1;
      return;
   }


   int flag = 0;
   //has name
   if (vectorOfTokens[line][0].type == UserSegment && vectorOfTokens[line][1].type == EndsKeyword)
   {
      lexems[line].hasInstruction = true;
      lexems[line].instrIndex = 1;
      lexems[line].hasOperands = false;
      return;

   }
   if (vectorOfTokens[line][0].type == Identifier || vectorOfTokens[line][0].type == UserSegment)
   {
      lexems[line].hasName = true;
      lexems[line].offset += 1;

   }
   // instruction check
   for (int i = 0; i <= 10; i++)
   {
      if (vectorOfTokens[line].size() == 1 && vectorOfTokens[line][0].type == Identifier)
      {
         vectorOfTokens[line][0].type = MacroName;
         lexems[line].hasInstruction = true;
         flag = 1;
         break;
      }
      if (vectorOfTokens[line][lexems[line].offset].type == instructionType[i])
      {
         lexems[line].hasInstruction = true;
         flag = 1;
         break;
      }
   }
   //flag for instruction check
   if (flag == 0)
   {
      lexems[line].hasInstruction = false;
      lexems[line].hasOperands = false;
      return;
   }
      //if we have an instruction
   else
   {
      lexems[line].instrIndex = lexems[line].offset;

      if (vectorOfTokens[line].size() <= lexems[line].offset + 1)
      {
         lexems[line].hasOperands = false;
         return;
      }
         //if we have operands
      else
      {
         lexems[line].offset += 1;
         lexems[line].hasOperands = true;

         lexems[line].operandIndices[1] = (lexems[line].offset);
         lexems[line].operandLengths[line] = 0;

         vector<end_token>::iterator itr;
         int i = (lexems[line].offset);

         for (int j = i; j < vectorOfTokens[line].size(); j++)
         {
            if (vectorOfTokens[line][j].type == Symbol && vectorOfTokens[line][j].token == ",")
            {
               lexems[line].operandIndices[2] = lexems[line].operandLengths[lexems[line].numberOfOperands + 1] +
                                                lexems[line].numberOfOperands + 1 + i;
               lexems[line].numberOfOperands += 1;
            }
            else
            {
               lexems[line].operandLengths[lexems[line].numberOfOperands + 1] += 1;
            }
         }

         return;
      }
   }
}

// Call cycle to create lexemes for each lines of tokens
void proceedTokens()
{
   emptyCheck();//delete empty str
   createSpaceVector(); //creat space for vector of lexemes

   // Create lexemes
   for (int i = 0; i < vectorOfTokens.size(); i++)
   {
      lexemeCreate(i);
   }

  // vector<int> to_delete;

   // MACROROSZIRENNYA
   for (int i = 0; i < vectorOfTokens.size(); i++)
   {
      // lexeme have macro
      struct macro* m = hasMacro(vectorOfTokens[i]);
      if (!m) continue;

      // Check parameters count
      if (vectorOfTokens[i].size() - 1 != m->parameters.size())
      {
         //std::cout << "Wrong macro parameters";
         continue;
      }

      map<string, end_token> param_replace;
      for (int p = 0; p < m->parameters.size(); p++)
         param_replace[m->parameters[p].token] = vectorOfTokens[i][p + 1];

      // Remove current lexeme
      //to_delete.push_back(i);

      // Insert macro lines
      for (int j = m->start+1 ; j < m->end; j++)
      {
         vectorOfTokens.insert(vectorOfTokens.begin() + i+1, vectorOfTokens[j]);
         lexems.insert(lexems.begin() + i +1, lexems[j]);

         // Replace parameters
         for (int k = 0; k < vectorOfTokens[j].size(); k++)
         {
            if (vectorOfTokens[j][k].type == Identifier)
            {
               auto a = param_replace.find(vectorOfTokens[j][k].token);
               if (a == param_replace.end()) continue;

               vectorOfTokens[i + j - m->start - 1][k] = a->second;
            }
         }
         i++;
      }
   }

  /* for (int i = to_delete.size() - 1; i >= 0; i--)
   {
      vectorOfTokens.erase(vectorOfTokens.begin() + to_delete[i]);
      lexems.erase(lexems.begin() + to_delete[i]);
   }*/
}

// Prints all token in a beautiful list
void printTokenList()
{
   string mas;
   char ch = '\n';

   for (int i = 0; i < vectorOfTokens.size(); i++)
   {
      cout << "Source string #:" << i + 1 << " :: ";
      for (int j = 0; j < vectorOfTokens[i].size(); j++)
         cout << vectorOfTokens[i][j].token << " ";
      cout << "\n";

      cout << "==================================================" << endl;
      cout << "#line" << "\t" << "#token" << "   " << "Token" << "\t" << "type of Token" << "\t\t||" << endl;
      cout << "==================================================" << endl;
      for (int j = 0; j < vectorOfTokens[i].size(); j++)
      {
         cout << "\t" << j << "\t" << vectorOfTokens[i][j].token << "\t: ("
              << tokenTypeToString(vectorOfTokens[i][j].type) << ")" << endl;

      }
      cout << "--------------------------------------------------" << "\n";
      cout << "TRANSCRIPT:(name field | mnemo | operand1| operand2|)" << "\n\n";
      cout << "LEXEM:  ";
      if (lexems[i].hasName || lexems[i].hasLabel) cout << "|" << "0" << "| ";
      if (!lexems[i].hasName && !lexems[i].hasLabel) cout << "|" << "---" << "|";
      if (lexems[i].hasInstruction) cout << " " << lexems[i].instrIndex << " " << "1" << " | ";
      if (!lexems[i].hasInstruction) cout << "|" << "---" << "|";
      if (lexems[i].hasOperands)
      {
         for (int k = 0; k <= lexems[i].numberOfOperands; k++)
         {
            cout << "  " << lexems[i].operandIndices[1 + k] << " " << lexems[i].operandLengths[1 + k] << " | ";
         }
      }
      else cout << "|" << "---" << "|" << " |" << "---" << "|";
      cout << endl;
      cout << "\n\n\n";
   }
}

void Lexem::SetError(string error, const end_token &end_token)
{
   this->error = error;
   this->error_token = end_token;
   this->has_error = true;
}
