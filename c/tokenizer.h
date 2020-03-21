#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdint.h>
#include "stddef.h"

#include "list.h"

typedef enum _token_type
{
   as
} token_type_t;

typedef struct _token
{
   char* string;

   token_type_t type;
   size_t line;

} token_t;

// Reads all bytes from file and returns null terminated string
uint8_t* t_read(const char* fn);

// Returns list of all tokens
list_t* t_tokenize(char* str);

#endif // TOKENIZER_H
