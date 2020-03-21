#include <stdio.h>

#include "tokenizer.h"
#include "lexeme.h"

#define TEST_FILE "../test.asm"

void print_et2_table(list_t* lexemes)
{
   for(size_t i = 0; i < lexemes->count; i++)
   {
      lexeme_t* lexeme = lexemes->collection[i];
      for(size_t j = 0; j < lexeme->tokens_cnt; j++)
         printf("%s ", lexeme->tokens[j].string);
      putchar('\n');

      printf("/========================================\\\n");
      printf("| N | String        | Type               |\n");
      printf("|---+---------------+--------------------|\n");

      for(size_t j = 0; j < lexeme->tokens_cnt; j++)
      {
         printf("| %-2li|  %-12s | %-19s|\n",
                j,
                lexeme->tokens[j].string,
                t_tt_to_name(lexeme->tokens[j].type));
      }
      printf("|========================================|\n");
      printf("| NAME |    MNEM   | OPERAND1 | OPERAND2 | \n");
      printf("|------+-----------+----------+----------|\n");

      char buff1[20];
      if(lexeme->has_label)
         snprintf(buff1, sizeof(buff1), "0");
      else
         snprintf(buff1, sizeof(buff1), "-");

      char buff2[20];
      snprintf(buff2, sizeof(buff2), "-");

      char buff3[20];
      snprintf(buff3, sizeof(buff3), "-");

      char buff4[20];
      snprintf(buff4, sizeof(buff4), "-");

      if(lexeme->has_instruction)
      {
         snprintf(buff2, sizeof(buff2), "%2li   1",
               lexeme->instr_index);

         if(lexeme->operands_count >= 1)
            snprintf(buff3, sizeof(buff3), "%2li   %2li",
                  lexeme->operands_info[0].index, lexeme->operands_info[0].length);

         if(lexeme->operands_count > 1)
            snprintf(buff4, sizeof(buff4), "%2li   %2li",
                     lexeme->operands_info[1].index, lexeme->operands_info[1].length);
      }


      printf("|   %s  |  %-9s| %-9s| %-9s|\n", buff1, buff2, buff3, buff4);

      printf("\\========================================/\n");

      putchar('\n');
      putchar('\n');
   }
}

int main()
{
   char* text = (char*)t_read(TEST_FILE);
   list_t* lexemes = t_tokenize(text);
   for(size_t i = 0; i < lexemes->count; i++)
   {
      lexeme_t* lexeme = lexemes->collection[i];
      l_structure(lexeme);
   }

   print_et2_table(lexemes);

   return 0;
}
