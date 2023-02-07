#include "dynarray.h"
#include "memory.h"
#define INITIAL_CAPACITY 16


dynarray* dynarray_init(){
	dynarray* arr=kmalloc(sizeof(dynarray));
	arr->size=0;
	arr->capacity=INITIAL_CAPACITY;
	arr->data=kmalloc(arr->capacity*sizeof(void*));
	return arr;
}

void dynarray_add(dynarray *arr, void *elem){
	if(arr->size==arr->capacity){
		arr->capacity*=2;
		arr->data=krealloc(arr->data, arr->capacity*sizeof(void*));
	}
	arr->data[arr->size++] = elem;
}

void dynarray_remove_index(dynarray *arr, int index){
	memmove(arr->data+index,arr->data+index+1,(arr->size-index-1)*sizeof(void*));
	arr->size--;
}