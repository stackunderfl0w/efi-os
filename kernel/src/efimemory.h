#pragma once
#include "typedef.h"
#include "graphics.h"
typedef struct {
    UINT32                          Type;           // EFI_MEMORY_TYPE, Field size is 32 bits followed by 32 bit pad
    UINT32                          Pad;
    EFI_PHYSICAL_ADDRESS            PhysicalStart;  // Field size is 64 bits
    EFI_VIRTUAL_ADDRESS             VirtualStart;   // Field size is 64 bits
    UINT64                          NumberOfPages;  // Field size is 64 bits
    UINT64                          Attribute;      // Field size is 64 bits
    uint64_t pad1;
} EFI_MEMORY_DESCRIPTOR;

/*typedef struct  {
    uint32_t Type;
    void* physAddr;
    void* virtAddr; 
    uint64_t NumberOfPages;
    uint64_t attribs;
}EFI_MEMORY_DESCRIPTOR;
*/
typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

extern const char* efi_memory_types[16];

uint64_t getMemorySize(EFI_MEMORY_DESCRIPTOR* memMap, uint64_t Entries, uint64_t DescSize);