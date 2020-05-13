#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <stdio.h>

#include "lexeme.h"

#define MAX_LABELS 100
#define MAX_SEGMENTS 5
#define MAX_VARIABLES 10

// Stores all necessary information about assembly file
typedef struct _assembly
{
   // Assembly`s lexemes
   list_t* lexemes;

   // User defined labels
   size_t labels_cnt;
   struct label
   {
      // Label defenition line
      size_t line;
      // Pointer to label's nane
      token_t* label;

   } labels[MAX_LABELS];

   // User defined segments
   size_t segments_cnt;
   struct segment
   {
      token_t* name;

      // Segment's size
      size_t size;

      // SEGMENT keyword line
      size_t line_start;
      // ENDS keyword line
      size_t line_end;

   } segments[MAX_SEGMENTS];

   // User defined variables
   size_t variables_cnt;
   struct variable
   {
      // Variable definition line
      size_t line;

      // Pointers to a variable defenition tokens
      token_t* name;
      token_t* type;
      token_t* value;

   } variable[MAX_VARIABLES];

} assembly_t;

// Allocates a new assembly object
assembly_t* a_create();

// Frees assembly object and all it's resources
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

// Allocates instruction buffers and calculates bytes
void a_second_pass(assembly_t* assembly);

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
