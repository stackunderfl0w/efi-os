#include "efimemory.h"
const char* efi_memory_types[] = {
	"EfiReservedMemoryType",
	"EfiLoaderCode",
	"EfiLoaderData",
	"EfiBootServicesCode",
	"EfiBootServicesData",
	"EfiRuntimeServicesCode",
	"EfiRuntimeServicesData",
	"EfiConventionalMemory",
	"EfiUnusableMemory",
	"EfiACPIReclaimMemory",
	"EfiACPIMemoryNVS",
	"EfiMemoryMappedIO",
	"EfiMemoryMappedIOPortSpace",
	"EfiPalCode",
	"EfiPersistentMemory",
	"EfiMaxMemoryType"
} ;

uint64_t getMemorySize(EFI_MEMORY_DESCRIPTOR* memMap, uint64_t Entries, uint64_t DescSize){
	uint64_t mem_size=0;
	for (int i = 0; i < Entries; ++i){
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)memMap + (i * DescSize));
		//kprintf("%s %ukb %u pages\n",efi_memory_types[desc->Type],desc->NumberOfPages*4,desc->NumberOfPages);
		if(desc->Type){
			mem_size+=4096*desc->NumberOfPages;
		}
	}
	return mem_size;
}