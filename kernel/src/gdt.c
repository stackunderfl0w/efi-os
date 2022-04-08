#include "gdt.h"

GDT Default_GDT = {
	{0, 0, 0, 0x00, 0x00, 0}, // null
	{0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
	{0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
	{0, 0, 0, 0x00, 0x00, 0}, // user null
	{0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
	{0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
};

void INIT_GDT(){
	GDT_Descriptor gdtDescriptor;
	gdtDescriptor.Size = sizeof(GDT) - 1;
	gdtDescriptor.Offset = (uint64_t)&Default_GDT;

	LoadGDT(&gdtDescriptor);
}