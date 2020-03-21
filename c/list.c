#ifdef __GNUC__
#pragma implementation "list.h"
#endif
#include "list.h"
// Taken from DeepCars =3

#include <string.h>
#include <assert.h>
#include <malloc.h>

//
// list_push()
//
inline void list_push(list_t* list, void* object)
{
   assert(list);
   assert(object);

   // current collection if full increase its size in 1.5 times
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
   assert(list);

   for (size_t i = 0; i < list->count; i++)
      free(list->collection[i]);
   list->count = 0;
}

//
// list_free()
//
void list_free(list_t* list)
{
   assert(list);

   if (list->collection != list->bootstrap) {
      free(list->collection);
   }
   free(list);
}

//
// list_remove()
//
void list_remove(list_t* list, void* object)
{
   assert(list);
   assert(object);

   //todo
}

//
// list_create
//
list_t* list_create()
{
   list_t* list = malloc(sizeof(list_t));
   list->count = 0;
   list->max_size = LIST_BOOTSTRAP_SIZE;
   list->collection = list->bootstrap;

   memset(list->bootstrap, 0, sizeof(list->bootstrap));

   return list;
}
