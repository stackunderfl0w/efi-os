#include "paging.h"
#include "stdio.h"
uint64_t FREE_MEMORY;
uint64_t USED_MEMORY;
uint64_t RESERVED_MEMORY;

char* pages_used;
uint64_t total_pages;
PL4* KERNEL_PL4;

void INIT_PAGING(EFI_MEMORY_DESCRIPTOR* memMap, uint64_t Entries, uint64_t DescSize, Framebuffer* buf){
	EFI_MEMORY_DESCRIPTOR* largest_segment;
	uint64_t largest_segment_size=0;
	for (int i = 0; i < Entries; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)memMap + (i * DescSize));
		if (desc->Type==EfiConventionalMemory){
			if(desc->NumberOfPages>largest_segment_size){
				largest_segment=desc;
				largest_segment_size=desc->NumberOfPages;
			}
		}
	}
	printf("largest_segment:%u\n",largest_segment_size);

	uint64_t total_mem=	getMemorySize(memMap, Entries, DescSize);
	FREE_MEMORY=total_mem;
	//print("placing page table man at ");
	pages_used=(char*)largest_segment->PhysicalStart;
	total_pages=total_mem/4096 +1;
	printf("Total memory pages:%u",total_pages);
	//maybe speed up with a memset
	for (uint64_t i = 0; i < total_pages; ++i){
		pages_used[i]=0;
	}
	// reserve between 0 and 0x100000(first megabyte)
	RESERVE_PAGES(0, 0x100);

	LOCK_PAGES(pages_used,total_pages/4096+1);
	for (uint64_t i = 0; i < Entries; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)memMap + (i * DescSize));
		if(desc->Type!=EfiConventionalMemory){
		//if(desc!=largest_segment){
			RESERVE_PAGES((void*)desc->PhysicalStart,desc->NumberOfPages);
		}
	}

	uint64_t kernel_size=((uint64_t)&_KernelEnd-(uint64_t)&_KernelStart)/4096;
	printf("kernel_size: %u\n",kernel_size);


	LOCK_PAGES(&_KernelStart,kernel_size);
	printf("Generating Kernel page table ");

	KERNEL_PL4=REQUEST_PAGE();
	//print("memseting kernel");
	memset(KERNEL_PL4,0,4096);
	KERNEL_PL4->entries[0].value=0;

	//print("maping display Framebuffer ");

	for (uint64_t i = (uint64_t)buf->BaseAddress; i < (uint64_t)buf->BaseAddress+buf->BufferSize; i+=4096){
		map_mem((void*)i,(void*)i);
	}

	for (uint64_t i = 0; i < total_mem; i+=4096){
		map_mem((void*)i,(void*)i);
	}	

	printf("Loading cr3");
	asm ("mov %0, %%cr3" : : "r" (KERNEL_PL4));
	printf("cr3 loaded\n");
}
//index that should not be after first free page
uint64_t page_index=0;
void* REQUEST_PAGE(){
	for ( ; page_index < total_pages; ++page_index){
		if(!pages_used[page_index]){
			LOCK_PAGE((void*)(page_index*4096));
			return (void*)(page_index*4096);
		}
	}
	return NULL;
}

void LOCK_PAGE(void* adr){
	uint64_t page=(uint64_t)adr/4096;
	if(!pages_used[page]){
		pages_used[page]=1;
		FREE_MEMORY-=4096;
		USED_MEMORY+=4096;
	}
}

void FREE_PAGE(void* adr){
	uint64_t page=(uint64_t)adr/4096;
	if(pages_used[page]){
		pages_used[page]=0;
		FREE_MEMORY+=4096;
		USED_MEMORY-=4096;
		page_index=MIN(page_index,page);
	}
}

void RESERVE_PAGE(void* adr){
	uint64_t page=(uint64_t)adr/4096;
	if(!pages_used[page]){
		pages_used[page]=1;
		FREE_MEMORY-=4096;
		RESERVED_MEMORY+=4096;
	}
}
void UNRESERVE_PAGE(void* adr){
	uint64_t page=(uint64_t)adr/4096;
	if(pages_used[page]){
		pages_used[page]=0;
		FREE_MEMORY+=4096;
		RESERVED_MEMORY-=4096;
		page_index=MIN(page_index,page);
	}
}

void LOCK_PAGES(void* adr, uint64_t count){
	for (int i = 0; i < count; ++i){
		LOCK_PAGE(adr+(i*4096));
	}
}
void FREE_PAGES(void* adr, uint64_t count){
	for (int i = 0; i < count; ++i){
		FREE_PAGE(adr+(i*4096));
	}
}
void RESERVE_PAGES(void* adr, uint64_t count){
	for (int i = 0; i < count; ++i){
		RESERVE_PAGE(adr+(i*4096));
	}
}
void UNRESERVE_PAGES(void* adr, uint64_t count){
	for (int i = 0; i < count; ++i){
		UNRESERVE_PAGE(adr+(i*4096));
	}
}

uint64_t get_free_memory(){
	return FREE_MEMORY;
}
uint64_t get_used_memory(){
	return USED_MEMORY;
}
uint64_t get_reserved_memory(){
	return RESERVED_MEMORY;
}

void map_mem(void* virtadr, void* physadr){
	//virtadr is 64 bits. 4 level page table doesnt map top 12 bits.
	//the last 12 bits are also not important as they are inside the page we are mapping.
	//the remaining 36 bits are split between the 4 levels.
	uint64_t virt=(uint64_t)virtadr;
	virt >>= 12;
	uint64_t PT_ofset = virt & 0x1ff;
	virt >>= 9;
	uint64_t PD_ofset = virt & 0x1ff;
	virt >>= 9;
	uint64_t PDP_ofset = virt & 0x1ff;
	virt >>= 9;
	uint64_t PL4_ofset = virt & 0x1ff;
	Page_Table_Entry* PL4E=&KERNEL_PL4->entries[PL4_ofset];
	//check if this part of virtual memory has been mapped yet
	if (!PT_GET_FLAG(PL4E,PT_Present)){

		void* temp=REQUEST_PAGE();
		memset(temp,0,4096);
		PT_SET_FLAG(PL4E,PT_Present);
		PT_SET_FLAG(PL4E,PT_RW);
		PT_SET_ADR(PL4E,(uint64_t)temp);


	}
	PL3* PDP=(void*)PT_GET_ADR(PL4E);



	Page_Table_Entry* PL3E=&PDP->entries[PDP_ofset];
	if (!PT_GET_FLAG(PL3E,PT_Present)){
		void* temp=REQUEST_PAGE();
		memset(temp,0,4096);
		PT_SET_FLAG(PL3E,PT_Present);
		PT_SET_FLAG(PL3E,PT_RW);
		PT_SET_ADR(PL3E,(uint64_t)temp);

	}
	PL2* PD=(void*)PT_GET_ADR(PL3E);



	Page_Table_Entry* PL2E=&PD->entries[PD_ofset];
	if (!PT_GET_FLAG(PL2E,PT_Present)){
		void* temp=REQUEST_PAGE();
		memset(temp,0,4096);
		PT_SET_FLAG(PL2E,PT_Present);
		PT_SET_FLAG(PL2E,PT_RW);
		PT_SET_ADR(PL2E,(uint64_t)temp);

	}
	PL1* PT=(void*)PT_GET_ADR(PL2E);


	Page_Table_Entry* PL1E=&PT->entries[PT_ofset];
	
	PT_SET_FLAG(PL1E,PT_Present);
	PT_SET_FLAG(PL1E,PT_RW);
	PT_SET_ADR(PL1E,(uint64_t)physadr);

	//void* page=(void*)PT_GET_ADR(PL1E);
}

void PT_SET_FLAG(Page_Table_Entry* PT, char flag){
	PT->value|= 1UL << flag;
}
void PT_RESET_FLAG(Page_Table_Entry* PT, char flag){
	PT->value &= ~(1UL << flag);
}
bool PT_GET_FLAG(Page_Table_Entry* PT, char flag){
	return (PT->value >> flag) & 1U;
}
void PT_SET_ADR(Page_Table_Entry* PT, uint64_t adr){
	PT->value&= 0xfff0000000000fff;
	PT->value|= 0x000ffffffffff000&adr;
}
uint64_t PT_GET_ADR(Page_Table_Entry* PT){
	return (PT->value& 0x000ffffffffff00);
}