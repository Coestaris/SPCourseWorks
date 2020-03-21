#ifndef LEXEME_H
#define LEXEME_H

#include <stddef.h>
#include <stdbool.h>

#include "tokenizer.h"

typedef enum _lexeme_type
{
   ff
} lexeme_type_t;

typedef struct _lexeme
{
   size_t tokens_cnt;
   token_t tokens[20];
   lexeme_type_t type;
   size_t line;

   bool has_instruction;
   bool has_label;
   size_t instr_index;
   size_t operands_count;
   struct {
      size_t index;
      size_t length;

   } operands_info[5];

} lexeme_t;

lexeme_t* l_create(size_t line);

void l_structure(lexeme_t* lexeme);

#endif // LEXEME_H
