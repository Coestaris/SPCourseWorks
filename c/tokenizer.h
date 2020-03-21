#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdint.h>
#include "stddef.h"

#include "list.h"

typedef enum _token_type
{
   TT_INSTRUCTION,

   TT_PTR,
   TT_BYTE,
   TT_WORD,
   TT_DWORD,

   TT_DB_DIRECTIVE,
   TT_DW_DIRECTIVE,
   TT_DD_DIRECTIVE,

   TT_SEGMENT_DIRECTIVE,
   TT_END_DIRECTIVE,
   TT_ENDS_DIRECTIVE,

   TT_REGISTER_SEG,
   TT_REGISTER32,
   TT_REGISTER8,
   TT_SYMBOL,

   TT_IDENTIFIER,

   TT_NUMBER2,
   TT_NUMBER10,
   TT_NUMBER16,

   TT_USERSEG,
   TT_LABEL,

   TT_UNKNOWN

} token_type_t;

// Contains information about single code word
typedef struct _token
{
   char* string;
   token_type_t type;

} token_t;

// Reads all bytes from file and returns null terminated string
uint8_t* t_read(const char* fn);

// Returns list of all tokens
list_t* t_tokenize(char* str);

// Converts token type its mnemonic name
const char* t_tt_to_name(token_type_t type);

#endif // TOKENIZER_H
