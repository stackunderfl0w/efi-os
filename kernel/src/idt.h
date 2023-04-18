#pragma once
#include "typedef.h"
#define INTERRUPT_GATE 0x8E
#define TRAP_GATE 0x8F
#define USER_SYSCALL_GATE 0xEE

//64-bit Interrupt Gate: 0x8E (p=1, dpl=0b00, type=0b1110 => type_attributes=0b1000_1110=0x8E)
//64-bit Trap Gate: 0x8F (p=1, dpl=0b00, type=0b1111 => type_attributes=1000_1111b=0x8F) 
typedef struct  {
	uint16_t Offset_0;
	uint16_t Segment_Selector;
	uint8_t Reserved_0;
	uint8_t Gate;
	uint16_t Offset_1;
	uint32_t Offset_2;
	uint32_t Reserved_1;
	
}__attribute__((packed)) IDT_ENTRY;

void IDT_Set_Offset(IDT_ENTRY *entry, size_t adr);
size_t IDT_Get_Offset(IDT_ENTRY *entry);

//gate
//bit 0-3 Type (interupt gate =0x8E, Trap gate =0x8F)
//bit 4 zero
//bit 5-6 DPL (sets privledge level)
//bit 7 Present(must be 1)

typedef struct {
	uint16_t Size;
	uint64_t Offset;
} __attribute__((packed))IDT_Descriptor;



void create_idt_entry(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector);

extern void Pit_Handler_Asm();
extern void interupt3();
extern void int80();

void create_interrupts();



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



