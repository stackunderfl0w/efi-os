#include "typedef.h"
typedef struct{
	void **data;
	int size;
	int capacity;
}dynarray;
dynarray* dynarray_init();

void dynarray_add(dynarray *arr, void *elem);

void dynarray_remove_index(dynarray *arr, int index);