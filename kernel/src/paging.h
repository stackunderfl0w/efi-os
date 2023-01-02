#pragma once
#include "typedef.h"
#include "graphics.h"
#include "efimemory.h"

#define MEMORY_FREE			0b0
#define MEMORY_LOCKED		0b01
#define MEMORY_RESERVED		0b10

#define PT_Present				1
#define PT_RW					1<<1
#define PT_UserSuper			1<<2
#define PT_WriteThrough			1<<3
#define PT_CacheDisabled		1<<4
#define PT_Accessed				1<<5
#define PT_Dirty				1<<6
#define PT_PageAttributeTable 	1<<7
#define PT_LargerPages			1<<7
#define PT_Global				1<<8

#define PT_Custom0				1<<9
#define PT_Custom1				1<<10
#define PT_Custom2				1<<11

#define PT_ExecuteDisable 		1<<63


typedef struct {
	uint64_t value;
}Page_Table_Entry;

typedef struct {
	Page_Table_Entry entries[512];
}Page_Table;

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;


void INIT_PAGING(EFI_MEMORY_DESCRIPTOR* memMap, uint64_t Entries, uint64_t DescSize, Framebuffer* buf);

void* REQUEST_PAGE();

void request_mapped_pages(void* virtadr, uint64_t bytes);


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


void PT_SET_FLAG(Page_Table_Entry* PT, uint64_t flag);
bool PT_GET_FLAG(Page_Table_Entry* PT, uint64_t flag);
void PT_SET_ADR(Page_Table_Entry* PT, uint64_t adr);
uint64_t PT_GET_ADR(Page_Table_Entry* PT);

void map_mem(void* virtadr, void* physadr);
void map_pages(void* virtadr, void* physadr ,uint64_t pages);
void map_allocated_mem(void* virtadr);

extern Page_Table* KERNEL_PL4;
