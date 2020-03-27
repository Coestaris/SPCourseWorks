#include <stdio.h>

#include "tokenizer.h"
#include "lexeme.h"
#include "assembly.h"

#define TEST_FILE "../test.asm"

void print_et2_table(assembly_t* assembly)
{
   for(size_t i = 0; i < assembly->lexemes->count; i++)
   {
      lexeme_t* lexeme = assembly->lexemes->collection[i];
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
                     lexeme->operands_info[0].op_index, lexeme->operands_info[0].op_length);

         if(lexeme->operands_count > 1)
            snprintf(buff4, sizeof(buff4), "%2li   %2li",
                     lexeme->operands_info[1].op_index, lexeme->operands_info[1].op_length);
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
   assembly_t* assembly = a_create();

   a_first_stage(assembly, text);
   //print_et2_table(assembly);

   a_first_pass(assembly);

   a_free(assembly);
   return 0;
}
