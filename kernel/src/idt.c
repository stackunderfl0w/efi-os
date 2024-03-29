#include "idt.h"
#include "io.h"
#include "interrupt_handlers.h"
#include "mouse.h"
#include "filesystem.h"

void IDT_Set_Offset(IDT_ENTRY *entry, size_t adr){
		entry->Offset_0 = (uint16_t)(adr&0xffff);
		entry->Offset_1 = (uint16_t)(((adr>>16)&0xffff) );
		entry->Offset_2 = (uint32_t)(((adr>>32)&0xffffffff));
	}
size_t IDT_Get_Offset(IDT_ENTRY *entry){
	return  (size_t)entry->Offset_2<<32|
			(size_t)entry->Offset_1<<16|
			(size_t)entry->Offset_0;
}
//gate
//bit 0-3 Type (interupt gate =0x8E, Trap gate =0x8F)
//bit 4 zero
//bit 5-6 DPL (sets privledge level)
//bit 7 Present(must be 1)

#define slots 0xFF
IDT_ENTRY idt_entries[slots];


void create_idt_entry(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector){
	IDT_ENTRY* interupt=&idt_entries[entryOffset];
	IDT_Set_Offset(interupt,(uint64_t)handler);
	interupt->Gate=type_attr;
	interupt->Segment_Selector=selector;
}

extern void Pit_Handler_Asm();
extern void interupt3();
void create_interrupts(){
	for (int i = 0; i < 256; ++i){
		create_idt_entry((void*)Unhandled_Handler, 0x03, INTERRUPT_GATE, 0x08);
	}
	create_idt_entry((void*)interupt3, 0x03, INTERRUPT_GATE, 0x08);
	create_idt_entry((void*)PageFault_Handler, 0x0E, INTERRUPT_GATE, 0x08);
	create_idt_entry((void*)GeneralFault_Handler, 0x0D, INTERRUPT_GATE, 0x08);
	create_idt_entry((void*)DoubleFault_Handler, 0x08, INTERRUPT_GATE, 0x08);
	create_idt_entry((void*)Keyboard_Handler, 0x21, INTERRUPT_GATE, 0x08);
	create_idt_entry((void*)Mouse_Handler, 0x2C, INTERRUPT_GATE, 0x08);

	//create_idt_entry((void*)PIT_Handler, 0x20, INTERRUPT_GATE, 0x08);
	create_idt_entry((void*)Pit_Handler_Asm, 0x20, INTERRUPT_GATE, 0x08);

	//filesystem interupts
	create_idt_entry((void*)int80, 0x80, USER_SYSCALL_GATE, 0x08);

	//create_idt_entry((void*)getcwd, 0x81, INTERRUPT_GATE, 0x08);


	IDT_Descriptor idt_descriptor;
	
	idt_descriptor.Size = sizeof(IDT_ENTRY)*slots - 1;
	idt_descriptor.Offset=(uint64_t)&idt_entries[0];



	asm ("lidt %0" : : "m" (idt_descriptor));

	PIC_remap(PIC1,PIC1+8);
	//InitPS2Mouse();
	outb(PIC1_DATA, 0b11111000);
	outb(PIC2_DATA, 0b11101111);

	//outb(PIC1_DATA, 0b11111000);
	//outb(PIC2_DATA, 0b11101111);
}

/*Master PIC

	IRQ 0 – system timer (cannot be changed)
	IRQ 1 – keyboard on PS/2 port (cannot be changed)
	IRQ 2 – cascaded signals from IRQs 8–15 (any devices configured to use IRQ 2 will actually be using IRQ 9)
	IRQ 3 – serial port controller for serial port 2 (shared with serial port 4, if present)
	IRQ 4 – serial port controller for serial port 1 (shared with serial port 3, if present)
	IRQ 5 – parallel port 3 or sound card
	IRQ 6 – floppy disk controller
	IRQ 7 – parallel port 1 (shared with parallel port 2, if present). It is used for printers or for any parallel port if a printer is not present. It can also be potentially be shared with a secondary sound card with careful management of the port.

Slave PIC

	IRQ 8 – real-time clock (RTC)
	IRQ 9 – Advanced Configuration and Power Interface (ACPI) system control interrupt on Intel chipsets.[3] Other chipset manufacturers might use another interrupt for this purpose, or make it available for the use of peripherals (any devices configured to use IRQ 2 will actually be using IRQ 9)
	IRQ 10 – The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
	IRQ 11 – The Interrupt is left open for the use of peripherals (open interrupt/available, SCSI or NIC)
	IRQ 12 – mouse on PS/2 port
	IRQ 13 – CPU co-processor  or  integrated floating point unit  or  inter-processor interrupt (use depends on OS)
	IRQ 14 – primary ATA channel (ATA interface usually serves hard disk drives and CD drives)
	IRQ 15 – secondary ATA channel
	*/