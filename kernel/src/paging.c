#include "paging.h"
#include "stdio.h"
uint64_t FREE_MEMORY;
uint64_t USED_MEMORY;
uint64_t RESERVED_MEMORY;

char* pages_used;
uint64_t total_pages;
Page_Table* KERNEL_PL4;

void map_fb_pages(void* virtadr, void* physadr ,uint64_t pages);

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
	printf("Generating Kernel page table\n");

	KERNEL_PL4=REQUEST_PAGE();
	printf("memseting kernel\n");
	memset(KERNEL_PL4,0,4096);

	printf("identity mapping mem\n");
	map_pages(0,0,total_mem/4096);

	printf("maping display Framebuffer, fb at:%p\n",buf->BaseAddress);
	//map_pages(buf->BaseAddress,buf->BaseAddress,buf->BufferSize/4096);
	map_fb_pages(buf->BaseAddress,buf->BaseAddress,buf->BufferSize/4096);


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
void map_mem_with_flags(void* virtadr, void* physadr,uint64_t flags){
	//virtadr is 64 bits. 4 level page table only maps bottom 48 bits
	//the bottom 12 bits are irelevant as they are inside the page we are mapping.
	//the remaining 36 bits are split between the 4 levels.
	//in 5 level paging another 9 bit level is added to make it 57 bit adressing
	uint64_t virt=(uint64_t)virtadr;
	//all the different page table levels have different names but seem to be structurally identical
	uint64_t PL4_ofset = virt>>39 & 0x1ff;
	uint64_t PDP_ofset = virt>>30 & 0x1ff;
	uint64_t PD_ofset = virt>>21 & 0x1ff;
	uint64_t PT_ofset = virt>>12 & 0x1ff;

	uint64_t offsets[4]={PL4_ofset,PDP_ofset,PD_ofset,PT_ofset};

	Page_Table* current_PL=KERNEL_PL4;
	Page_Table_Entry* current_entry;

	//navigate down through the tables
	for (int i = 0; i < 3; ++i){
		//get next entry from current table
		Page_Table_Entry* current_entry=&current_PL->entries[offsets[i]];
		if (!PT_GET_FLAG(current_entry,PT_Present)){
			void* temp=REQUEST_PAGE();
			memset(temp,0,4096);
			PT_SET_FLAG(current_entry,PT_Present);
			PT_SET_FLAG(current_entry,PT_RW);
			PT_SET_ADR(current_entry,(uint64_t)temp);
		}
		current_PL=(void*)PT_GET_ADR(current_entry);
	}

	Page_Table_Entry* PL1E=&current_PL->entries[PT_ofset];

	PT_SET_FLAG(PL1E,flags);
	PT_SET_ADR(PL1E,(uint64_t)physadr);

	//void* page=(void*)PT_GET_ADR(PL1E);
}
void map_mem(void* virtadr, void* physadr){
	map_mem_with_flags(virtadr, physadr,PT_RW|PT_Present);
}
void map_pages(void* virtadr, void* physadr ,uint64_t pages){
	for (int i = 0; i < pages; i++){
		map_mem(virtadr+i*4096,physadr+i*4096);
	}
}
//add framebuffer to memory map with pages marked as write combining? Further investigation needed
void map_fb_pages(void* virtadr, void* physadr ,uint64_t pages){
	for (int i = 0; i < pages; i++){
		map_mem_with_flags(virtadr+i*4096,physadr+i*4096,PT_Present|PT_RW|PT_WriteThrough|PT_CacheDisabled|PT_PageAttributeTable);
	}
}
//request pages to be mapped at location rouded up to page size
void request_mapped_pages(void* virtadr, uint64_t bytes){
	for (int i = 0; i < bytes; i+=4096){
		map_mem(virtadr+i,REQUEST_PAGE());
	}
}
void PT_SET_FLAG(Page_Table_Entry* PT, uint64_t flag){
	PT->value|= flag;
}
void PT_RESET_FLAG(Page_Table_Entry* PT, char flag){
	PT->value &= ~flag;
}
bool PT_GET_FLAG(Page_Table_Entry* PT, uint64_t flag){
	return (PT->value & flag);
}
void PT_SET_ADR(Page_Table_Entry* PT, uint64_t adr){
	PT->value&= 0xfff0000000000fff;
	PT->value|= 0x000ffffffffff000&adr;
}
uint64_t PT_GET_ADR(Page_Table_Entry* PT){
	return (PT->value& 0x000ffffffffff00);
}