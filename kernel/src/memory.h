#pragma once
#include "typedef.h"
#include "paging.h"
//needed to include pointer to same type
typedef struct HEAP_SEG_HEADER HEAP_SEG_HEADER;

struct HEAP_SEG_HEADER{
	HEAP_SEG_HEADER* previous_hdr;
	HEAP_SEG_HEADER* next_hdr;
	uint64_t len;
	bool free;
};

void INIT_HEAP(void* adr, uint64_t pages);

void* kmalloc(uint64_t size);

void* kcalloc(uint64_t size);

void* krealloc(void* adr, uint64_t size);

void free(void* adr);
