#ifdef __GNUC__
#pragma implementation "lexeme.h"
#endif
#include "lexeme.h"

#include <malloc.h>
#include <string.h>

lexeme_t* l_create(size_t line)
{
   lexeme_t* l = malloc(sizeof(lexeme_t));
   memset(l, 0, sizeof(lexeme_t));
   l->line = line;
   return l;
}

void l_structure(lexeme_t* lexeme)
{
   size_t off = 0;

   // has label
   if(lexeme->tokens_cnt >= 2 &&
      lexeme->tokens[0].type == TT_IDENTIFIER &&
      lexeme->tokens[1].type == TT_SYMBOL &&
      lexeme->tokens[1].string[0] == ':')
   {
      lexeme->has_label = true;
      off += 2; // lbl :
   }

   if(lexeme->tokens_cnt <= off)
   {
      // only label
      lexeme->has_instruction = false;
      return;
   }

   if(lexeme->tokens[off].type == TT_IDENTIFIER)
   {
      //has name
      lexeme->has_label = true;
      off += 1;
   }

   if(lexeme->tokens[off].type == TT_INSTRUCTION ||
      lexeme->tokens[off].type == TT_DW_DIRECTIVE ||
      lexeme->tokens[off].type == TT_DB_DIRECTIVE ||
      lexeme->tokens[off].type == TT_DD_DIRECTIVE ||
      lexeme->tokens[off].type == TT_END_DIRECTIVE ||
      lexeme->tokens[off].type == TT_ENDS_DIRECTIVE ||
      lexeme->tokens[off].type == TT_SEGMENT_DIRECTIVE)
   {
      lexeme->has_instruction = true;
   }
   else
   {
      return;
   }

   lexeme->instr_index = off;
   off += 1;

   if(lexeme->tokens_cnt <= off)
   {
      // has instruction only
      return;
   }

   lexeme->operands_info[0].index = off;
   for(size_t i = off; i < lexeme->tokens_cnt; i++)
   {
      token_t* t = &lexeme->tokens[i];
      if(t->type == TT_SYMBOL && t->string[0] == ',')
      {
         lexeme->operands_info[lexeme->operands_count + 1].index =
               lexeme->operands_info[lexeme->operands_count].length + lexeme->operands_count + 1 + off;
         lexeme->operands_count++;
      }
      else
      {
         lexeme->operands_info[lexeme->operands_count].length += 1;
      }
   }

   lexeme->operands_count++;
}