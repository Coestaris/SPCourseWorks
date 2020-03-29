#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <stdio.h>

#include "lexeme.h"

// Stores all necessary information about assembly file
typedef struct _assembly
{
   // Assembly`s lexemes
   list_t* lexemes;

   // User defined labels
   size_t  labels_cnt;
   struct label
   {
      size_t line;
      token_t* label;

   } labels[100];

   // User defined segments
   size_t segments_cnt;
   struct segment
   {
      token_t* name;
      size_t size;

      size_t line_start;
      size_t line_end;

   }  segments[5];

   // User defined variables
   size_t variables_cnt;
   struct variable
   {
      size_t line;
      token_t* name;
      token_t* type;
      token_t* value;

   }  variable[10];

} assembly_t;

// Allocates new assembly object
assembly_t* a_create();

// Frees assembly object and all its resources
void a_free(assembly_t* assembly);

// Prints errors
void a_errors(assembly_t* assembly, FILE* stream);

//
// KR Task functions
//
// Performs all necessary operation for the first stage.
// Parse tokens and determine their types
void a_first_stage(assembly_t* assembly, char* data);

// Performs all necessary operation for the first pass.
// Get information about labels, segments and variables.
// Analyze instruction operands and calculate instructions sizes
void a_first_pass(assembly_t* assembly);

//
// Useful assembly getters
//
// Get user segment by its name
struct segment* a_get_segment(assembly_t* assembly, char* name);

// Get user segment by line index
struct segment* a_get_segment_by_line(assembly_t* assembly, size_t line);

// Get user label by its name
struct label* a_get_label(assembly_t* assembly, char* name);

// Get user variable by its declaration line
struct variable* a_get_variable_by_line(assembly_t* assembly, size_t line);

// Get user variable by its name
struct variable* a_get_variable(assembly_t* assembly, char* name);

// Get lexeme by its declaration name
lexeme_t* a_get_lexeme_by_line(assembly_t* assembly, size_t line);

#endif // ASSEMBLY_H
