#ifdef __GNUC__
#pragma implementation "assembly.h"
#endif
#include "assembly.h"

#include <malloc.h>
#include <string.h>

#include "errors.h"

// Useful macro for iterating through lexemes
#define LEX_LP_BEG \
    for(size_t i = 0; i < assembly->lexemes->count; i++)    \
    {                                                       \
      lexeme_t* lexeme = assembly->lexemes->collection[i];
#define LEX_LP_END \
    }

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
   e_assert(assembly, "Passed NULL argument");

   // Split input string by \n and parse lines to lexemes
   assembly->lexemes = t_tokenize(data);

   // Analyze lexeme's structure
   LEX_LP_BEG
      if(!lexeme->err)
         l_structure(lexeme);
   LEX_LP_END
}

//
// a_first_pass()
//
void a_first_pass(assembly_t* assembly)
{
   e_assert(assembly, "Passed NULL argument");

   // Analyze lexeme types
   LEX_LP_BEG
      if(!lexeme->err)
         l_fetch_lexeme_type(lexeme);
   LEX_LP_END

   // Create segments, labels and vars
   LEX_LP_BEG
   {
      if(!lexeme->err)
      {
         if (lexeme->type == LT_SEGMENT_DEFINITION)
         {
            struct segment* seg = a_get_segment(assembly, lexeme->tokens[0].string);
            if(seg)
            {
               T_SE(lexeme, "Same segment already declared", 0);
               continue;
            }

            seg = &assembly->segments[assembly->segments_cnt++];
            seg->name = &lexeme->tokens[0];
            seg->line_start = lexeme->line;
         }
         else if (lexeme->type == LT_SEGMENT_END)
         {
            struct segment* seg = a_get_segment(assembly, lexeme->tokens[0].string);
            if(!seg)
            {
               T_SE(lexeme, "Unknown segment", 1);
               continue;
            }
            seg->line_end = lexeme->line;
         }
         else if (lexeme->type == LT_VAR_DEFINITION)
         {
            struct variable* var = a_get_variable(assembly, lexeme->tokens[0].string);
            if(var)
            {
               T_SE(lexeme, "Same var already declared", 0);
               continue;
            }

            var = &assembly->variable[assembly->variables_cnt++];
            var->line = lexeme->line;
            var->name = &lexeme->tokens[0];
            var->type = &lexeme->tokens[1];
            var->value = &lexeme->tokens[2];
         }
         else if (lexeme->type == LT_LABEL || (lexeme->type == LT_INSTRUCTION && lexeme->has_label))
         {
            struct label* label = a_get_label(assembly, lexeme->tokens[0].string);
            if(label)
            {
               T_SE(lexeme, "Same label already declared", 0);
               continue;
            }

            label = &assembly->labels[assembly->labels_cnt++];
            label->line = lexeme->line;
            label->label = &lexeme->tokens[0];
         }
         else if(lexeme->type == LT_INSTRUCTION)
         {
            //Instruction without instruction
            if(!lexeme->has_instruction)
               T_SE(lexeme, "Met unexpected token", 0);
         }
         else
         {
            T_SE(lexeme, "Met unexpected token", 0);
         }
      }
   };
   LEX_LP_END

   // Segment of current lexeme
   struct segment* active_segment = NULL;

   // Analyze lexeme operands and assign instructions
   LEX_LP_BEG
   {
      if(!lexeme->err)
      {
         if (lexeme->type == LT_INSTRUCTION)
         {
            if(!l_fetch_op_info(lexeme, assembly))
               continue;

            if(!l_assign_instruction(lexeme))
               continue;
         }

         // If we encounter segment redefinition, this means that
         // the following instructions will be in another segment
         if (lexeme->type == LT_SEGMENT_DEFINITION)
            active_segment = a_get_segment_by_line(assembly, lexeme->line);

         // Calculate instruction offset
         lexeme->offset = active_segment->size;
         active_segment->size += l_get_size(lexeme, assembly);
         if(lexeme->err) continue;
      }
   }
   LEX_LP_END

}

//
// a_free()
//
void a_free(assembly_t* assembly)
{
   e_assert(assembly, "Passed NULL argument");

   LEX_LP_BEG
      l_free(lexeme);
   LEX_LP_END

   list_free(assembly->lexemes);
   free(assembly);
}

//
// a_get_segment()
//
struct segment* a_get_segment_by_line(assembly_t* assembly, size_t line)
{
   e_assert(assembly, "Passed NULL argument");

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
   e_assert(assembly, "Passed NULL argument");
   e_assert(name, "Passed NULL argument");

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
   e_assert(assembly, "Passed NULL argument");
   e_assert(name, "Passed NULL argument");

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
   e_assert(assembly, "Passed NULL argument");
   e_assert(name, "Passed NULL argument");

   for(size_t i = 0; i < assembly->variables_cnt; i++)
   {
      if(!strcmp(assembly->variable[i].name->string, name))
         return &assembly->variable[i];
   }

   return NULL;
}

//
// a_get_variable_by_line()
//
struct variable* a_get_variable_by_line(assembly_t* assembly, size_t line)
{
   e_assert(assembly, "Passed NULL argument");

   for(size_t i = 0; i < assembly->variables_cnt; i++)
   {
      if(assembly->variable[i].line == line)
         return &assembly->variable[i];
   }

   return NULL;
}

//
// a_get_lexeme_by_line()
//
lexeme_t* a_get_lexeme_by_line(assembly_t* assembly, size_t line)
{
   e_assert(assembly, "Passed NULL argument");

   LEX_LP_BEG
      if(lexeme->line == line)
         return lexeme;
   LEX_LP_END

   return NULL;
}

//
// a_errors()
//
void a_errors(assembly_t* assembly, FILE* stream)
{
   size_t errors = 0;
   LEX_LP_BEG
      if(lexeme->err)
      {
         fprintf(stream, "Error at line %li (%li) near token \"%s\": %s\n", i, lexeme->line,
                 lexeme->tokens[lexeme->err_tk].string, lexeme->err);
         errors++;
      }
   LEX_LP_END
   fprintf(stream, "\n%li errors\n", errors);
}