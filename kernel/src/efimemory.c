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
		//print(efi_memory_types[desc->Type]);
		//printchar(' ');
		//print(to_string(desc->NumberOfPages*4));
		//print("kb");
		//print(to_string(desc->Type));
		//printchar(' ');

		//print(to_string(desc->NumberOfPages));
		//printchar(' ');
		if(desc->Type){
			mem_size+=4096*desc->NumberOfPages;
		}
	}
	return mem_size;
}