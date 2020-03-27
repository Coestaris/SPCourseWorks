#ifdef __GNUC__
#pragma implementation "assembly.h"
#endif
#include "assembly.h"

#include <malloc.h>
#include <string.h>
#include <assert.h>

// Useful macro for iterating through lexemes
#define LEX_LP_BEG                                                         \
                   for(size_t i = 0; i < assembly->lexemes->count; i++)    \
                   {                                                       \
                     lexeme_t* lexeme = assembly->lexemes->collection[i];
#define LEX_LP_END }

//
// a_create()
//
assembly_t* a_create()
{
   assembly_t* assembly = malloc(sizeof(assembly_t));
   memset(assembly, 0, sizeof(assembly_t));
   return assembly;
}

//
// a_first_stage()
//
void a_first_stage(assembly_t* assembly, char* data)
{
   assert(assembly);

   assembly->lexemes = t_tokenize(data);
   LEX_LP_BEG
      l_structure(lexeme);
   LEX_LP_END
}

//
// a_first_pass()
//

void a_first_pass(assembly_t* assembly)
{
   assert(assembly);

   // Analyze lexeme types
   LEX_LP_BEG
      l_fetch_lexeme_type(lexeme);
   LEX_LP_END

   // Create segments, labels and vars
   LEX_LP_BEG
   {
      if(lexeme->type == LT_SEGMENT_DEFINITION)
      {
         struct segment* seg = &assembly->segments[assembly->segments_cnt++];
         seg->name = &lexeme->tokens[0];
         seg->line_start = lexeme->line;
      }
      else if(lexeme->type == LT_SEGMENT_END)
      {
         struct segment* seg = a_get_segment(assembly, lexeme->tokens[0].string);
         seg->line_end = lexeme->line;
      }
      else if(lexeme->type == LT_VAR_DEFINITION)
      {
         struct variable* var = &assembly->variable[assembly->variables_cnt++];
         var->name = &lexeme->tokens[0];
         var->type = &lexeme->tokens[1];
         var->value = &lexeme->tokens[2];
      }
      else if(lexeme->type == LT_LABEL || (lexeme->type == LT_INSTRUCTION && lexeme->has_label))
      {
         struct label* label = &assembly->labels[assembly->labels_cnt++];
         label->line = lexeme->line;
         label->label = &lexeme->tokens[0];
      }
   };
   LEX_LP_END

   // Analyze lexeme operands and assign instructions
   LEX_LP_BEG
      if(lexeme->type == LT_INSTRUCTION)
      {
         l_fetch_op_info(lexeme, assembly);
         l_assign_instruction(lexeme);
      }
   LEX_LP_END

}

//
// a_free()
//

void a_free(assembly_t* assembly)
{
   assert(assembly);

   for(size_t i = 0; i < assembly->lexemes->count; i++)
   {
      lexeme_t* lexeme = assembly->lexemes->collection[i];
      l_free(lexeme);
   }
   list_free(assembly->lexemes);
   free(assembly);
}

//
// a_get_segment()
//
struct segment* a_get_segment_by_line(assembly_t* assembly, size_t line)
{
   assert(assembly);

   for(size_t i = 0; i < assembly->segments_cnt; i++)
   {
      if(assembly->segments[i].line_start <= line && line <= assembly->segments[i].line_end)
         return &assembly->segments[i];
   }

   return NULL;
}

//
// a_get_segment()
//
struct segment* a_get_segment(assembly_t* assembly, char* name)
{
   assert(assembly);
   assert(name);

   for(size_t i = 0; i < assembly->segments_cnt; i++)
   {
      if(!strcmp(assembly->segments[i].name->string, name))
         return &assembly->segments[i];
   }

   return NULL;
}

//
// a_get_label()
//
struct label* a_get_label(assembly_t* assembly, char* name)
{
   assert(assembly);
   assert(name);

   for(size_t i = 0; i < assembly->labels_cnt; i++)
   {
      if(!strcmp(assembly->labels[i].label->string, name))
         return &assembly->labels[i];
   }

   return NULL;
}

//
// a_get_variable()
//
struct variable* a_get_variable(assembly_t* assembly, char* name)
{
   assert(assembly);
   assert(name);

   for(size_t i = 0; i < assembly->variables_cnt; i++)
   {
      if(!strcmp(assembly->variable[i].name->string, name))
         return &assembly->variable[i];
   }

   return NULL;
}
