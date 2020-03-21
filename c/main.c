#include <stdio.h>

#include "tokenizer.h"

#define TEST_FILE "../test.asm"

int main()
{
   char* text = (char*)t_read(TEST_FILE);

   list_t* tokens = t_tokenize(text);
   for(size_t i = 0; i < tokens->count; i++)
   {
      token_t* t = tokens->collection[i];
      printf("%s\n", t->string);
   }
   return 0;
}
