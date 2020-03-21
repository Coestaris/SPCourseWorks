#ifndef LIST_H
#define LIST_H
// Taken from DeepCars =3

#include <stddef.h>

// Startup size of the bootstrap
#define LIST_BOOTSTRAP_SIZE 50
#define LIST_SIZE_INCREASE 1.5f

// Generic list to store pointers
typedef struct _list
{
   // Current count of elements in collection
   size_t count;
   // Actual size of allocated pointer
   size_t max_size;
   // Bootstrap optimization
   void* bootstrap[LIST_BOOTSTRAP_SIZE];
   // Pointer to a data storing memory
   void** collection;

} list_t;

// Push new element to a list
void list_push(list_t* list, void* object);

// Remove element from list by it's pointer
void list_remove(list_t* list, void* object);

// Free all lists elements
void list_free_elements(list_t* list);

// Free list but keep all its elements
void list_free(list_t* list);

// list_t constructor
list_t* list_create();

#endif // LIST_H
