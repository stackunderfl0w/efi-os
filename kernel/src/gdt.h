#pragma once
#include <stdint.h>

typedef struct  {
    uint16_t Limit0;
    uint16_t Base0;
    uint8_t Base1;
    uint8_t AccessByte;
    uint8_t Limit1_Flags;
    uint8_t Base2;
}__attribute__((packed)) GDT_Entry;

typedef struct  {
    GDT_Entry Null; //0x00
    GDT_Entry KernelCode; //0x08
    GDT_Entry KernelData; //0x10
    GDT_Entry UserNull;
    GDT_Entry UserCode;
    GDT_Entry UserData;
} __attribute__((packed)) 
__attribute((aligned(0x1000)))GDT;

typedef struct {
    uint16_t Size;
    uint64_t Offset;
} __attribute__((packed))GDT_Descriptor;


extern GDT Default_GDT;


#ifdef __cplusplus
extern "C" {
#endif
void LoadGDT(GDT_Descriptor* gdtDescriptor);

void INIT_GDT();