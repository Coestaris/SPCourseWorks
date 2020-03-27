#ifdef __GNUC__
#pragma implementation "assembly.h"
#endif
#include "assembly.h"

#include <malloc.h>
#include <string.h>
#include <assert.h>

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
   for(size_t i = 0; i < assembly->lexemes->count; i++)
   {
      lexeme_t* lexeme = assembly->lexemes->collection[i];
      l_structure(lexeme);
   }
}

//
// a_first_pass()
//

void a_first_pass(assembly_t* assembly)
{
   assert(assembly);
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

struct segment* a_get_segment(assembly_t* assembly, char* name)
{
   assert(assembly);
   assert(name);

   return NULL;
}

//
// a_get_label()
//
struct label* a_get_label(assembly_t* assembly, char* name)
{
   assert(assembly);
   assert(name);

   return NULL;
}

//
// a_get_variable()
//
struct variable* a_get_variable(assembly_t* assembly, char* name)
{
   assert(assembly);
   assert(name);

   return NULL;
}
