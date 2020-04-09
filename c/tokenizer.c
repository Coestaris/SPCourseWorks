#ifdef __GNUC__
#pragma implementation "tokenizer.h"
#endif
#include "tokenizer.h"

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexeme.h"
#include "errors.h"

// Converts string to integer with specified base
static int64_t string_to_num(char* string, size_t len, uint8_t base)
{
   static const char* literals = "0123456789ABCDEF";
   bool has_sign = string[0] == '-';

   int64_t result = 0;
   for(int64_t i = len - 1; i >= has_sign ? 1 : 0; i--)
   {
      int8_t literal = -1;
      for(size_t j = 0; j < 16; j++)
         if(literals[j] == string[i])
         {
            literal = j;
            break;
         }

      e_assert(literal != -1, "Unable to find specified literal");
      e_assert(literal < base, "Used wrong literal for specified base");

      result += pow(base, (double)len - i - 1) * (int64_t)literal;
   }

   if(has_sign)
      return -result;

   return result;
}

static bool is_whitespace(char c)
{
   return c == '\r' || c == ' ' || c == '\n' || c == '\t' || c == '\0';
}

// Split symbols for source.
static bool is_delimiter(char s)
{
   return is_whitespace(s) || s == '*' || s == ',' || s == '[' || s == ']' || s == '+' || s == '=' || s == ':';
}

// Remove whitespaces from both sides
static char* trim_local_buffer(char* buffer)
{
   size_t l = strlen(buffer);
   if(l == 0)
      return buffer;

   size_t start = 0;
   size_t end = l - 1;
   while(is_whitespace(buffer[start]))
   {
      start++;
      if(start == end + 1)
         return buffer + start;
   }
   while(is_whitespace(buffer[end]))
   {
      buffer[end] = 0;
      end--;
      if(end == start)
         return buffer + start;
   }
   return buffer + start;
}

static struct {
   const char* string;
   token_type_t type;
   const char* type_str;
} types[] =
{
      { "CLC",     TT_INSTRUCTION, "INSTRUCTION" },
      { "NEG",     TT_INSTRUCTION, "INSTRUCTION" },
      { "DEC",     TT_INSTRUCTION, "INSTRUCTION" },
      { "ADD",     TT_INSTRUCTION, "INSTRUCTION" },
      { "TEST",    TT_INSTRUCTION, "INSTRUCTION" },
      { "AND",     TT_INSTRUCTION, "INSTRUCTION" },
      { "MOV",     TT_INSTRUCTION, "INSTRUCTION"},
      { "CMP",     TT_INSTRUCTION, "INSTRUCTION" },
      { "JNBE",    TT_INSTRUCTION, "INSTRUCTION" },
      { "JMP",     TT_INSTRUCTION, "INSTRUCTION" },

      { "PTR",     TT_PTR, "PTR_KEYWORD" },
      { "BYTE",    TT_BYTE, "BYTE_KEYWORD" },
      { "WORD",    TT_WORD, "WORD_KEYWORD" },
      { "DWORD",   TT_DWORD, "DWORD_KEYWORD" },

      { "DB",      TT_DB_DIRECTIVE, "DB_DIRECTIVE" },
      { "DW",      TT_DW_DIRECTIVE, "DW_DIRECTIVE" },
      { "DD",      TT_DD_DIRECTIVE, "DD_DIRECTIVE" },

      { "SEGMENT", TT_SEGMENT_DIRECTIVE, "SEGMENT_DIRECTIVE" },
      { "ENDS",    TT_ENDS_DIRECTIVE, "ENDS_DIRECTIVE" },
      { "END",     TT_END_DIRECTIVE, "END_DIRECTIVE" },

      { "ES",     TT_REGISTER_SEG, "REGISTER_SEG" },
      { "DS",     TT_REGISTER_SEG, "REGISTER_SEG" },
      { "FS",     TT_REGISTER_SEG, "REGISTER_SEG" },
      { "SS",     TT_REGISTER_SEG, "REGISTER_SEG" },
      { "GS",     TT_REGISTER_SEG, "REGISTER_SEG" },
      { "CS",     TT_REGISTER_SEG, "REGISTER_SEG" },

      { "EAX",     TT_REGISTER32, "REGISTER32" },
      { "EBX",     TT_REGISTER32, "REGISTER32" },
      { "ECX",     TT_REGISTER32, "REGISTER32" },
      { "EDX",     TT_REGISTER32, "REGISTER32" },
      { "EDI",     TT_REGISTER32, "REGISTER32" },
      { "ESI",     TT_REGISTER32, "REGISTER32" },
      { "EBP",     TT_REGISTER32, "REGISTER32" },
      { "ESP",     TT_REGISTER32, "REGISTER32" },

      { "AL",      TT_REGISTER8, "REGISTER8" },
      { "AH",      TT_REGISTER8, "REGISTER8" },
      { "BL",      TT_REGISTER8, "REGISTER8" },
      { "BH",      TT_REGISTER8, "REGISTER8" },
      { "CL",      TT_REGISTER8, "REGISTER8" },
      { "CH",      TT_REGISTER8, "REGISTER8" },
      { "DL",      TT_REGISTER8, "REGISTER8" },
      { "DH",      TT_REGISTER8, "REGISTER8" },

      { ":",       TT_SYMBOL, "SYMBOL" },
      { "[",       TT_SYMBOL, "SYMBOL" },
      { "]",       TT_SYMBOL, "SYMBOL" },
      { "*",       TT_SYMBOL, "SYMBOL" },
      { "+",       TT_SYMBOL, "SYMBOL" },
      { ",",       TT_SYMBOL, "SYMBOL" },
};

// Size of token-type match table
static const size_t type_count = sizeof(types) / sizeof(types[0]);

// Returns true if string is valid identifier
static bool is_id(char* string)
{
   if(isdigit(string[0]))
      return false;

   return strlen(string) <= 4;
}

// Returns true if string is value binary number
static bool is_bin(char* string)
{
   bool has_sign = string[0] == '-';

   size_t len = strlen(string);
   for(size_t i = has_sign ? 1 : 0; i < len - 1; i++)
      if(string[i] != '0' && string[i] != '1')
         return false;

   return string[len - 1] == 'B';
}

// Returns true if string is value decimal number
static bool is_dec(char* string)
{
   bool has_sign = string[0] == '-';

   size_t len = strlen(string);
   for(size_t i = has_sign ? 1 : 0; i < len; i++)
      if((string[i] < '0' || string[i] > '9'))
         return false;

   return true;
}

// Returns true if string is value hexadecimal number
static bool is_hex(char* string)
{
   bool has_sign = string[0] == '-';

   size_t len = strlen(string);
   for(size_t i = has_sign ? 1 : 0; i < len - 1; i++)
      if(!((string[i] >= '0' && string[i] <= '9') || (string[i] >= 'A' && string[i] <= 'H')))
         return false;

   return string[len - 1] == 'H';
}

// Fills token info and determines its type
static token_t t_create(char* string, size_t line)
{
   e_assert(string, "Passed NULL argument");

   token_t t;
   t.string = strdup(string);
   t.type = TT_UNKNOWN;

   // Determining token type
   for(size_t i = 0; i < type_count; i++)
      if(!strcmp(string, types[i].string))
      {
         t.type = types[i].type;
         break;
      }

   if(t.type == TT_UNKNOWN)
   {
      if(is_hex(string))
         t.type = TT_NUMBER16;
      else if(is_dec(string))
         t.type = TT_NUMBER10;
      else if(is_bin(string))
         t.type = TT_NUMBER2;
      else if(is_id(string))
         t.type = TT_IDENTIFIER;
   }

   return t;
}

//
// t_tt_to_name()
//
const char* t_tt_to_name(token_type_t type)
{
   for(size_t i = 0; i < type_count; i++)
      if(type == types[i].type)
         return types[i].type_str;

   switch(type)
   {
      case TT_IDENTIFIER:
         return "IDENTIFIER";
      case TT_REGISTER32:
         return "REGISTER32";
      case TT_REGISTER8:
         return "REGISTER8";
      case TT_NUMBER2:
         return "NUMBER2";
      case TT_NUMBER10:
         return "NUMBER10";
      case TT_NUMBER16:
         return "NUMBER16";
      default:
         return "unknown";
   }
}

//
// t_read()
//
uint8_t* t_read(const char* fn)
{
   e_assert(fn, "Passed NULL argument");

   FILE* f = fopen(fn, "r");
   e_assert(f, "Unable to open file");

   fseek(f, 0, SEEK_END);
   size_t len = ftell(f);
   fseek(f, 0, SEEK_SET);

   uint8_t* data = malloc(len + 1);
   data[len] = 0;

   e_assert(fread(data, len, 1, f) == 1, "Unable to read data from file");
   fclose(f);

   return data;
}

//
// t_tokenize()
//
list_t* t_tokenize(char* str)
{
   e_assert(str, "Passed NULL argument");

   size_t len = strlen(str);

   char buffer[100];
   size_t buff_cnt = 0;
   size_t line_cnt = 0;

   lexeme_t* curr_lex = l_create(0);
   list_t* r = list_create();

   for(size_t i = 0; i < len; i++)
   {
      char c = str[i];

      if(is_delimiter(c))
      {
         // 0 terminate buffer so we can work with it
         buffer[buff_cnt] = 0;
         char* trimmed_buff = trim_local_buffer(buffer);
         if(strlen(trimmed_buff) != 0)
         {
            token_t tk = t_create(trimmed_buff, line_cnt);
            if(tk.type == TT_UNKNOWN)
               T_SE(curr_lex, "Unknown token type", curr_lex->tokens_cnt);
            curr_lex->tokens[curr_lex->tokens_cnt++] = tk;
         }

         // Make string from char
         buffer[0] = c;
         buffer[1] = 0;
         trimmed_buff = trim_local_buffer(buffer);
         if(strlen(trimmed_buff) != 0)
         {
            token_t tk = t_create(trimmed_buff, line_cnt);
            if(tk.type == TT_UNKNOWN)
               T_SE(curr_lex, "Unknown token type", curr_lex->tokens_cnt);
            curr_lex->tokens[curr_lex->tokens_cnt++] = tk;
         }
         buff_cnt = 0;
      }
      else buffer[buff_cnt++] = (char)toupper(c);

      if(c == '\n')
      {
         line_cnt++;
         if(curr_lex->tokens_cnt)
         {
            list_push(r, curr_lex);
            curr_lex = l_create(line_cnt);
         }
      }
   }

   // last token
   char* trimmed_buff = trim_local_buffer(buffer);
   if(strlen(trimmed_buff) != 0)
   {
      token_t tk = t_create(trimmed_buff, line_cnt);
      if(tk.type == TT_UNKNOWN)
      T_SE(curr_lex, "Unknown token type", curr_lex->tokens_cnt);
      curr_lex->tokens[curr_lex->tokens_cnt++] = tk;
   }

   if(curr_lex->tokens_cnt)
      list_push(r, curr_lex);
   else
      l_free(curr_lex);

   return r;
}

//
// t_num()
//
int64_t t_num(token_t* token)
{
   e_assert(token, "Passed NULL argument");

   if(token->type == TT_NUMBER2)
      return string_to_num(token->string, strlen(token->string) - 1, 2);
   else if(token->type == TT_NUMBER10)
      return string_to_num(token->string, strlen(token->string), 10);
   else if(token->type == TT_NUMBER16)
      return string_to_num(token->string, strlen(token->string) - 1, 16);

   e_err("Unable to convert token to integer");
}