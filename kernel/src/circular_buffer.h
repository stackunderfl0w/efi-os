#pragma once
#include "typedef.h"

typedef struct{
	size_t max_size;
	size_t element_size;
	_Atomic size_t size;
	_Atomic size_t head;
	_Atomic size_t tail;
	void* data;
	_Atomic bool locked;
}circular_buffer;

circular_buffer* cb_init(size_t size, size_t element_size);

void cb_free(circular_buffer* cb);

size_t cb_push(circular_buffer* cb, const void* items, size_t count);

size_t cb_pop(circular_buffer* cb, void* items, size_t count);
