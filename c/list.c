#if __linux__
#ifdef __GNUC__
#pragma implementation "list.h"
#endif
#else
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "list.h"

#include <string.h>
#include <malloc.h>

#include "errors.h"

//
// list_push()
//
void list_push(list_t* list, void* object)
{
   e_assert(list, "Passed NULL argument");
   e_assert(object, "Passed NULL argument");

   // if current collection is full we increase its size in LIST_SIZE_INCREASE times
   if (list->count > list->max_size - 1)
   {
      if (list->max_size == LIST_BOOTSTRAP_SIZE)
      {
         size_t new_len = (int) ((float) list->max_size * LIST_SIZE_INCREASE);

         void** new_collection = malloc(sizeof(void*) * new_len);
         memcpy(new_collection, list->bootstrap, list->count * sizeof(void*));
         list->collection = new_collection;
         list->max_size = new_len;
      }
      else
      {
         size_t new_len = (int) ((float) list->max_size * LIST_SIZE_INCREASE);
         list->collection = realloc(list->collection, sizeof(void*) * new_len);

         e_assert(list->collection, "Unable to reallocate collection");

         list->max_size = new_len;
      }
   }
   list->collection[list->count++] = object;
}

//
// list_free_elements()
//
void list_free_elements(list_t* list)
{
   e_assert(list, "Passed NULL argument");

   for (size_t i = 0; i < list->count; i++)
      free(list->collection[i]);
   list->count = 0;
}

//
// list_free()
//
void list_free(list_t* list)
{
   e_assert(list, "Passed NULL argument");

   if (list->collection != list->bootstrap)
   {
      free(list->collection);
   }
   free(list);
}

//
// list_remove()
//
void list_remove(list_t* list, void* object)
{
   e_assert(list, "Passed NULL argument");
   e_assert(object, "Passed NULL argument");

   //todo
}

//
// list_create
//
list_t* list_create()
{
   list_t* list = malloc(sizeof(list_t));

   e_assert(list, "Unable to allocate list");

   list->count = 0;
   list->max_size = LIST_BOOTSTRAP_SIZE;
   list->collection = list->bootstrap;

   memset(list->bootstrap, 0, sizeof(list->bootstrap));

   return list;
}
