#ifndef LEXEME_H
#define LEXEME_H

#include <stddef.h>
#include <stdbool.h>

#include "tokenizer.h"

// Mnemonic type of the lexeme
typedef enum _lexeme_type
{
   LT_SEGMENT_DEFINITION,
   LT_VAR_DEFINITION,
   LT_SEGMENT_END,
   LT_LABEL,
   LT_INSTRUCTION,
   LT_END

} lexeme_type_t;

// Mnemonic type of each lexeme operand
typedef enum _operand_type
{
   OT_REG8,
   OT_REG32,
   OT_MEM,
   OT_MEM8,
   OT_MEM32,
   OT_LABEL_FORWARD,
   OT_LABEL_BACKWARD,
   OT_IMM8,
   OT_IMM32

} operand_type_t;

// Represent assembly line
typedef struct _lexeme
{
   // Lines tokens
   size_t tokens_cnt;
   token_t tokens[100];

   // Line index
   size_t line;

   // Type of the lexeme. Set by l_fetch_lexeme_type()
   lexeme_type_t type;

   // Lexeme structure. Set by l_structure()
   bool has_instruction;
   bool has_label;
   size_t instr_index;
   size_t operands_count;

   // Information about operands. Set by l_fetch_op_info()
   struct {
      // Index of the operand in lexeme.tokens
      size_t op_index;
      // Count of tokens in current operand
      size_t op_length;

      // Type of the current operand
      operand_type_t type;

      // For immX: Given Constant
      // For regX: Given register
      // For labelX: Given label
      // For memX set to NULL
      token_t* operand;

      // Tokens of OT_MEM, OT_MEM8 or OT_MEM32
      // dword  ptr  ES : [ edx + esi * 4 + 6 ]
      //         1    2      3     4    5   6
      // 1 - Type keyword. Could be NULL
      // 2 - Segment. Could be NULL
      // 3 - Base register. Always set
      // 4 - Index register. Always set
      // 5 - Scale. Always set.
      // 6 - Displacement. Always set
      token_t* type_keyword;
      token_t* segment;
      token_t* base;
      token_t* index;
      token_t* scale;
      token_t* disp;

   } operands_info[5];

} lexeme_t;

// Allocates new lexeme
lexeme_t* l_create(size_t line);

// Frees lexeme
void l_free(lexeme_t* lexeme);

// Determines lexeme's type
void l_fetch_lexeme_type(lexeme_t* lexeme);

// Gets all necessary information about instruction operands according to KR task
void l_fetch_op_info(lexeme_t* lexeme);

// Determines structure of the instruction
void l_structure(lexeme_t* lexeme);

#endif // LEXEME_H
