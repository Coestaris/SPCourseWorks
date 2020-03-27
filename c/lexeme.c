#ifdef __GNUC__
#pragma implementation "lexeme.h"
#endif
#include "lexeme.h"

#include <malloc.h>
#include <string.h>
#include <assert.h>

/*
 * Current insructions:
 * clc
 *   F8 CLC
 *
 * neg reg
 *    F6 /3 NEG r/m8
 *    F7 /3 NEG r/m32
 *
 * dec mem
 *    FE /1 DEC r/m8
 *    FF /1 DEC r/m32
 *
 * add reg, imm
 *    80 /0 ib ADD r/m8, imm8
 *    83 /0 ib ADD r/m32, imm8
 *    81 /0 id ADD r/m32, imm32
 *
 * test reg, mem
 *    84 /r	TEST r8, r/m8
 *    85 /r	TEST r32, r/m32
 *
 * and mem,reg
 *    20 /r AND r/m8, r8
 *    21 /r AND r/m32, r32
 *
 * mov reg, reg
 *    88 /r MOV r/m8, r8
 *    89 /r MOV r/m32, r32
 *
 * cmp mem,imm
 *    80 /7 ib CMP r/m8, imm8
 *    83 /7 ib CMP r/m32, imm8
 *    81 /7 id CMP r/m32, imm32
 *
 * jnbe
 *       77 cb JNBE rel8
 *    OF 87 cw JNBE rel16
 *
 * jmp
 *    EB cb JMP rel8
 *    E9 cw JMP rel16
*/

//
// l_create()
//
lexeme_t* l_create(size_t line)
{
   lexeme_t* l = malloc(sizeof(lexeme_t));
   memset(l, 0, sizeof(lexeme_t));
   l->line = line;
   return l;
}


//
// l_free()
//
void l_free(lexeme_t* lexeme)
{
   assert(lexeme);

   free(lexeme);
}

//
// l_fetch_lexeme_type()
//
void l_fetch_lexeme_type(lexeme_t* lexeme)
{
   assert(lexeme);

   if(   lexeme->tokens_cnt == 2 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         lexeme->tokens[1].type == TT_SEGMENT_DIRECTIVE)
      lexeme->type = LT_SEGMENT_DEFINITION;
   else if(lexeme->tokens_cnt == 2 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         lexeme->tokens[1].type == TT_ENDS_DIRECTIVE)
      lexeme->type = LT_SEGMENT_END;
   else if(lexeme->tokens_cnt == 3 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         (lexeme->tokens[1].type == TT_DB_DIRECTIVE || lexeme->tokens[1].type == TT_DW_DIRECTIVE || lexeme->tokens[1].type == TT_DD_DIRECTIVE))
      lexeme->type = LT_VAR_DEFINITION;
   else if(lexeme->tokens_cnt == 2 &&
         lexeme->tokens[0].type == TT_IDENTIFIER &&
         lexeme->tokens[1].string[0] == ':')
      lexeme->type = LT_LABEL;
   else if(lexeme->tokens_cnt == 1 &&
         lexeme->tokens[0].type == TT_END_DIRECTIVE)
      lexeme->type = LT_END;
   else
      lexeme->type = LT_INSTRUCTION;
}

//
// l_fetch_op_info()
//
void l_fetch_op_info(lexeme_t* lexeme)
{
   assert(lexeme);

}

//
// l_structure()
//
void l_structure(lexeme_t* lexeme)
{
   assert(lexeme);

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

   lexeme->operands_info[0].op_index = off;
   for(size_t i = off; i < lexeme->tokens_cnt; i++)
   {
      token_t* t = &lexeme->tokens[i];
      if(t->type == TT_SYMBOL && t->string[0] == ',')
      {
         lexeme->operands_info[lexeme->operands_count + 1].op_index =
               lexeme->operands_info[lexeme->operands_count].op_length + lexeme->operands_count + 1 + off;
         lexeme->operands_count++;
      }
      else
      {
         lexeme->operands_info[lexeme->operands_count].op_length += 1;
      }
   }

   lexeme->operands_count++;
}