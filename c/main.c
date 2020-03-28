#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "tokenizer.h"
#include "lexeme.h"
#include "assembly.h"
#include "errors.h"

#define TEST_FILE "../test.asm"
#define OUT_FILE "../out.log"

static void print_et2_table(FILE* output, assembly_t* assembly)
{
   for(size_t i = 0; i < assembly->lexemes->count; i++)
   {
      lexeme_t* lexeme = assembly->lexemes->collection[i];
      for(size_t j = 0; j < lexeme->tokens_cnt; j++)
         fprintf(output, "%s ", lexeme->tokens[j].string);
      putc('\n', output);

      fprintf(output, "/========================================\\\n");
      fprintf(output, "| N | String        | Type               |\n");
      fprintf(output, "|---+---------------+--------------------|\n");

      for(size_t j = 0; j < lexeme->tokens_cnt; j++)
      {
         fprintf(output, "| %-2li|  %-12s | %-19s|\n",
                j,
                lexeme->tokens[j].string,
                t_tt_to_name(lexeme->tokens[j].type));
      }
      fprintf(output, "|========================================|\n");
      fprintf(output, "| NAME |    MNEM   | OPERAND1 | OPERAND2 | \n");
      fprintf(output, "|------+-----------+----------+----------|\n");

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


      fprintf(output, "|   %s  |  %-9s| %-9s| %-9s|\n", buff1, buff2, buff3, buff4);

      fprintf(output, "\\========================================/\n");

      putc('\n', output);
      putc('\n', output);
   }
}

// Converts integer to a string and fills empty symbols with specified.
static void extend_integer(char* buffer, size_t length, int64_t value, char fill, bool hex)
{
   memset(buffer, 0, length);
   snprintf(buffer, length + 1, hex ? "%lX" : "%li", value);
   size_t l = strlen(buffer);

   memmove(buffer + length - l, buffer, l + 1);
   memset(buffer, fill, length - l);
}

static void print_et3_table(FILE* output, assembly_t* assembly)
{
   char lex_str[200];
   char lidx_str[3];
   char offset_str[9];

   fputs("OFFSET TABLE\n", output);
   fputs("/====================================================================\\\n", output);
   fputs("| LN |  OFFSET  |                        SOURCE                      |\n", output);
   fputs("|----|----------|----------------------------------------------------|\n", output);
   for(size_t i = 0; i < assembly->lexemes->count; i++)
   {
      lexeme_t* lexeme = assembly->lexemes->collection[i];
      l_string(lex_str, sizeof(lex_str), lexeme);

      extend_integer(lidx_str, sizeof(lidx_str) - 1, i, '0', false);
      extend_integer(offset_str, sizeof(offset_str) - 1, lexeme->offset, '0', true);

      fprintf(output, "| %s | %s | %-50s |\n", lidx_str, offset_str, lex_str);
   }
   fputs("\\====================================================================/\n\n", output);

   fputs("SEGMENTS TABLE\n", output);
   fputs("/===================================\\\n", output);
   fputs("| ID |   SEGMENT  |   SIZE   |  BIT |\n", output);
   fputs("|----|------------|----------|------|\n", output);
   for(size_t i = 0; i < assembly->segments_cnt; i++)
   {
      struct segment* seg = &assembly->segments[i];

      extend_integer(lidx_str, sizeof(lidx_str) - 1, i, '0', false);
      extend_integer(offset_str, sizeof(offset_str) - 1, seg->size, '0', true);
      fprintf(output, "| %s |    %-6s  | %s |  32  |\n", lidx_str, seg->name->string, offset_str);
   }
   fputs("\\===================================/\n\n", output);

   fputs("USER DEFINED SYMBOLS\n", output);
   fputs("/=======================================\\\n", output);
   fputs("| ID |   SYMBOL  |   TYPE   |   VALUE   |\n", output);
   fputs("|----|-----------|----------|-----------|\n", output);
   size_t counter = 0;
   for(size_t i = 0; i < assembly->labels_cnt; i++)
   {
      struct label* lb = &assembly->labels[i];
      struct segment* seg = a_get_segment_by_line(assembly, lb->line);
      lexeme_t* lexeme = a_get_lexeme_by_line(assembly, lb->line);

      extend_integer(lidx_str, sizeof(lidx_str) - 1, counter, '0', false);
      extend_integer(offset_str, 4, lexeme->offset, '0', true);

      fprintf(output, "| %s |    %s   |   LABEL  | %4s:%4s |\n",
            lidx_str, lb->label->string, seg->name->string, offset_str);

      counter++;
   }

   for(size_t i = 0; i < assembly->variables_cnt; i++)
   {
      struct variable* var = &assembly->variable[i];
      struct segment* seg = a_get_segment_by_line(assembly, var->line);
      lexeme_t* lexeme = a_get_lexeme_by_line(assembly, var->line);

      extend_integer(lidx_str, sizeof(lidx_str) - 1, counter, '0', false);
      extend_integer(offset_str, 4, lexeme->offset, '0', true);

      const char* type = NULL;
      switch(var->type->type)
      {
         case TT_DB_DIRECTIVE: type = "BYTE";
            break;
         case TT_DW_DIRECTIVE: type = "WORD";
            break;
         case TT_DD_DIRECTIVE: type = "DWORD";
            break;
      }

      fprintf(output, "| %s |    %s   |   %-5s  | %4s:%4s |\n",
             lidx_str, var->name->string, type, seg->name->string, offset_str);

      counter++;
   }
   fputs("\\=======================================/\n\n", output);

   fputs("SEGMENTS DESTINATION TABLE\n", output);
   fputs("/=================================\\\n", output);
   fputs("| ID |   SEG REG  |  DESTINATION  |\n", output);
   fputs("|----|------------|---------------|\n", output);
   fprintf(output, "| 00 |     CS     |      %4s     |\n", assembly->segments[1].name->string);
   fprintf(output, "| 01 |     DS     |      %4s     |\n", assembly->segments[0].name->string);
   fprintf(output, "| 02 |     SS     |    NOTHING    |\n");
   fprintf(output, "| 03 |     ES     |    NOTHING    |\n");
   fprintf(output, "| 04 |     FS     |    NOTHING    |\n");
   fprintf(output, "| 05 |     GS     |    NOTHING    |\n");
   fputs("\\=================================/\n", output);

}

int main()
{
   e_set_out(stderr);

   FILE* log = fopen(OUT_FILE, "w");
   e_assert(log, "Unable to open file "OUT_FILE);

   char* text = (char*)t_read(TEST_FILE);
   assembly_t* assembly = a_create();

   a_first_stage(assembly, text);
   //print_et2_table(log, assembly);

   a_first_pass(assembly);
   print_et3_table(log, assembly);

   a_free(assembly);
   free(text);
   fclose(log);
   return 0;
}
