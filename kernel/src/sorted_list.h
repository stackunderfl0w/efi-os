#pragma once
#include "typedef.h"

typedef struct{
	int (*cmp)(void*,void*);
	int (*search_cmp)(void*,void*);
	void ** data;
	uint64_t size;
	uint64_t capacity;
}sorted_list;

sorted_list* create_sorted_list(int (*element_cmp)(void*,void*),int (*search_cmp)(void*,void*));

void sorted_list_free(sorted_list* l);

void resize_sorted_list(sorted_list* l, uint64_t new_size);

void reserve_sorted_list(sorted_list* l, uint64_t size);
void sorted_list_insert(sorted_list* l, void* element);
void sorted_list_remove(sorted_list* l, void* element);

uint64_t sorted_list_search(sorted_list* l, void* search);

void* sorted_list_get(sorted_list* l, void* search);