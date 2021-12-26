#include "typedef.h"
#include "paging.h"

typedef struct HEAP_SEG_HEADER HEAP_SEG_HEADER;

struct HEAP_SEG_HEADER{
	HEAP_SEG_HEADER* previous_hdr;
	HEAP_SEG_HEADER* next_hdr;
	uint64_t len;
	bool free;
};

//typedef struct{
//	void* previous_hdr;
//	void* next_hdr;
//	uint64_t len;
//	bool free;
//}HEAP_SEG_HEADER;


void INIT_HEAP(void* adr, uint64_t pages);

void* malloc();

void* calloc();

void free(void* adr);
/*			if(cur->len==size){
				cur->free=false;
				return (void*)(cur+1);
			}*/