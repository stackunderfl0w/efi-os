#pragma once
#include "typedef.h"
#include "graphics.h"
#include "efimemory.h"

#define MEMORY_FREE			0b0
#define MEMORY_LOCKED		0b01
#define MEMORY_RESERVED		0b10

void INIT_PAGING(EFI_MEMORY_DESCRIPTOR* memMap, uint64_t Entries, uint64_t DescSize, Framebuffer* buf);

void* REQUEST_PAGE();

void LOCK_PAGE(void* adr);
void FREE_PAGE(void* adr);
void RESERVE_PAGE(void* adr);
void UNRESERVE_PAGE(void* adr);

void LOCK_PAGES(void* adr, uint64_t count);
void FREE_PAGES(void* adr, uint64_t count);
void RESERVE_PAGES(void* adr, uint64_t count);
void UNRESERVE_PAGES(void* adr, uint64_t count);

uint64_t get_free_memory();
uint64_t get_used_memory();
uint64_t get_reserved_memory();

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;


enum PAGE_FLAGS{
	PT_Present,
	PT_RW,
	PT_UserSuper,
	PT_WriteThrough,
	PT_CacheDisabled,
	PT_Accessed,
	PT_LargerPages = 7,
	PT_Custom0 = 9,
	PT_Custom1,
	PT_Custom2,
	PT_NX = 63

};


typedef struct {
	uint64_t value;
}Page_Table_Entry;

typedef struct {
	Page_Table_Entry entries[512];
}Page_Table;

typedef Page_Table PL4;
typedef Page_Table PL3;
typedef Page_Table PL2;
typedef Page_Table PL1;



void PT_SET_FLAG(Page_Table_Entry* PT, char flag);
bool PT_GET_FLAG(Page_Table_Entry* PT, char flag);
void PT_SET_ADR(Page_Table_Entry* PT, uint64_t adr);
uint64_t PT_GET_ADR(Page_Table_Entry* PT);

void map_mem(void* virtadr, void* physadr);
extern PL4* KERNEL_PL4;
