#include "gdt.h"
//Limit0; Base0; Base1; AccessByte; Limit1_Flags; Base2;

GDT Default_GDT = {
	{0, 0, 0, 0x00, 0x00, 0}, // null
	{0, 0, 0, 0x9a, 0xa0, 0}, // kernel code segment
	{0, 0, 0, 0x92, 0xa0, 0}, // kernel data segment
	{0, 0, 0, 0x00, 0x00, 0}, // null
	{0, 0, 0, 0xfa, 0xa0, 0}, // user code segment
	{0, 0, 0, 0xf2, 0xa0, 0}, // user data segment
};

void INIT_GDT(){
	GDT_Descriptor gdtDescriptor;
	gdtDescriptor.Size = sizeof(GDT) - 1;
	gdtDescriptor.Offset = (uint64_t)&Default_GDT;

	LoadGDT(&gdtDescriptor);
}