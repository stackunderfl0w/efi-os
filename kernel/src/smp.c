#include "smp.h"
#include "stdio.h"
#include "pit.h"
uint8_t lapic_ids[256]={0}; // CPU core Local APIC IDs
uint8_t numcore=0;          // number of cores detected
uint64_t lapic_ptr=0;       // pointer to the Local APIC MMIO registers
uint64_t ioapic_ptr=0;      // pointer to the IO APIC MMIO registers
extern void core_wakeup();

void detect_cores(uint8_t *rsdt){
	printf("core wakeup %x\n",core_wakeup);
	uint8_t *ptr, *ptr2;
	uint32_t len;

	// iterate on ACPI table pointers
	for(len = *((uint32_t*)(rsdt + 4)), ptr2 = rsdt + 36; ptr2 < rsdt + len; ptr2 += rsdt[0]=='X' ? 8 : 4) {
		ptr = (uint8_t*)(uintptr_t)(rsdt[0]=='X' ? *((uint64_t*)ptr2) : *((uint32_t*)ptr2));
		if(!memcmp(ptr, "APIC", 4)) {
			// found MADT
			lapic_ptr = (uint64_t)(*((uint64_t*)(ptr+0x24)));
			ptr2 = ptr + *((uint32_t*)(ptr + 4));
			// iterate on variable length records
			for(ptr += 44; ptr < ptr2; ptr += ptr[1]) {
				switch(ptr[0]) {
					case 0: if(ptr[4] & 1) lapic_ids[numcore++] = ptr[3];// break; // found Processor Local APIC
					printf("found Processor Local APIC\n%p\n",lapic_ptr);
					break;
					case 1: ioapic_ptr = (uint64_t)*((uint32_t*)(ptr+4)); //break;  // found IOAPIC
					printf("found IOAPIC\n");
					break;
					case 5: lapic_ptr = *((uint64_t*)(ptr+4)); //break;             // found 64 bit LAPIC
					printf("found 64 bit LAPIC\n");
					break;
				}
			}
			break;
		}
	}
	printf("Found %u cores, IOAPIC %p, LAPIC %p, Processor IDs:", numcore, ioapic_ptr, lapic_ptr);
	for(int i = 0; i < numcore; i++)
		printf(" %u", lapic_ids[i]);
	printf("\n");
}
/*
3.6.5 APIC Memory Mapping
In a compliant system, all APICs must be implemented as memory-mapped I/O devices. APIC
base addresses are at the top of the memory address space. All APIC local units are mapped to the
same addresses, which are not shared. Each processor accesses its local APIC via these memory
addresses. The default base address for the local APICs is 0FEE0_0000h.
Unlike the local APICs, the I/O APICs are mapped to give shared access from all processors,
providing full symmetric I/O access. The default base address for the first I/O APIC is
0FEC0_0000h. Subsequent I/O APIC addresses are assigned in 4K increments. For example, the
second I/O APIC is at 0FEC0_1000h.
*/