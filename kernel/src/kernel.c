#include "typedef.h"
#include <elf.h>
#include "bitmap-font.h"
#include "graphics.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "efimemory.h"
#include "mouse.h"
#include "paging.h"
#include "memory.h"
#include "serial.h"
#include "ata-pio.h"
#include "fat.h"
#include "thread.h"
#include "scheduler.h"
#include "time.h"
#include "keyboard.h"
#include "shell.h"
#include "stdio.h"
#include "smp.h"
#include "tty.h"

typedef struct {
	Framebuffer* buf;
	bitmap_font* font;
	EFI_MEMORY_DESCRIPTOR* mem_map;
	UINTN map_size;
	UINTN map_desc_size;
	RSDPDescriptor* rsdp;
}bootinfo;

#ifndef __cplusplus
#include "loop.h"

#else
extern "C" {
#include "loop.h"
}

extern "C" 
#endif
graphics_context* k_context;

int _start(bootinfo *info){
	asm("cli");

	//init_serial();
	//print_serial("hello world");


	//manually create kernel stdout&tty so it can be created without heap and remains in scope
    graphics_context k_graphics=init_text_overlay(info->buf, info->font);
	k_context=&k_graphics;
	FILE std;
	char std_buf[8192];

    tty tty0=init_tty_0(k_context,&std,std_buf,8192);



	printchar(k_context,'a');

	printf("Bootloader exited successfully\n");

	INIT_GDT();
	printf("GDT loaded\n");
	
	create_interrupts();
	printf("Interupts loaded\n");

	//SET_PIT_DIVISOR(65535);
	SET_PIT_FREQUENCY(100);
	printf("Pit set\n");

	INIT_PS2_MOUSE();
	printf("PS2 mouse initialized\n");

	printf("Frambuffer base:%u\n",(uint64_t)info->buf->BaseAddress);

	uint32_t numEntries=info->map_size/info->map_desc_size;
	uint64_t memsize=getMemorySize(info->mem_map,numEntries,info->map_desc_size);

	printf("Total memory:%u(%uMB)\n",memsize,memsize/(1024*1024));

	INIT_PAGING(info->mem_map,numEntries,info->map_desc_size,info->buf);
	printf("Paging initialized\n");

	printf("Free memory: %ukb\n",get_free_memory()/1024);
	printf("Used memory: %ukb\n",get_used_memory()/1024);
	printf("Reserved memory: %ukb\n",get_reserved_memory()/1024);


	void* heap_location=(void*)0x8000000000;
	INIT_HEAP(heap_location,0x10);
	printf("Kernel Heap initialized at %p\n",heap_location);

	printf("Enabling interupts");

	asm("sti");

	float fl=123.456789;

	printf("%f\n",fl);

	INIT_FILESYSTEM();
	
	int file_entries;
	read_directory("/",&file_entries);
	printf("%f\n",fl);

	//sleep(2000);
	/*sleep(1);
	char* file =read_file("SCRCLR  ELF");
	for (int i = 0; i < 32; ++i)
	{
		printf("%x ",(uint64_t)*(file+i));
	}
	printf("%f\n",fl);*/

	//uint8_t* file = read_file("/resources/TEST    TXT");
	//uint8_t* file = read_file("/resources/startup.txt");

	//uint8_t* font2 = read_file("/resources/zap-light16.psf");

	//write_file("/resources/WRTTEST TXT",file,1024);


	//bitmap_font loaded_font=load_font(font2);

	//init_text_overlay(info->buf, &loaded_font);

	//sleep(2000);

	//INIT_RTC();


	/*CHAR8* program_space=malloc(0x16000);

	printf("space allocated at,%x",program_space);

	CHAR8* kern =  read_file("/SCRCLR  ELF");

	printf("Program loaded , size=%u\n",512);


	Elf64_Ehdr* header=(Elf64_Ehdr*)kern;
	printf("Arch: %u\n\n",(long)header->e_machine);
	for (int i = 0; i < 32; ++i)
	{
		long x=kern[i];
		printf("%x,  ",x);
	}


	printf("entry: %u\n",header->e_entry);
	printf("ofset: %u\n",header->e_phoff);

	Elf64_Phdr* phdrs=(Elf64_Phdr*)(kern+header->e_phoff);

	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header->e_phnum * header->e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header->e_phentsize)
	)
	{//load each program segment at the memory location indicated by its header in p_addr
		switch (phdr->p_type){
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				printf("Kernel p_paddr %u \n",phdr->p_paddr);

				//SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
				//map_mem(segment, REQUEST_PAGE());
				printf("%x",program_space +(uint64_t)segment);
				printf("%x",((uint64_t)segment));

				UINTN size = phdr->p_filesz;
				for (UINTN i = 0; i < size; ++i)
				{
					*(char*)(program_space+(uint64_t)segment+i)=kern[phdr->p_offset+i];
				}
				break;
			}
		}
	}
	printf("Kernel e_entry %u \n",header->e_entry);
	printf("Kernel entry %u \n",&header->e_entry);


	int (*KernelStart)() = ((__attribute__((sysv_abi)) int (*)() ) program_space+header->e_entry);

	printf("Kernel start %u \n",KernelStart);
	KernelStart(info->buf->BaseAddress);
	//char* nel=calloc(1024);
*/	//new_process("/SCRCLR  ELF",info->buf->BaseAddress);
	//atapio_write_sectors(0,1,file);
	//loop();

	//run_shell(info->buf, info->font);


	/*

	while(1){
		//printf("second:%u minute:%u hour:%u day:%u month:%u year:%u\n",
			//SYSTEM_TIME.second,SYSTEM_TIME.minute,SYSTEM_TIME.hour,SYSTEM_TIME.day,SYSTEM_TIME.month,SYSTEM_TIME.year);
		//printf("Day of week: %s\n",days_of_the_week[dayofweek()]);
		//printf("second:%u ",
		//SYSTEM_TIME.second);
		get_cursor_pos(&x, &y);

		move_cursor(40, 0);
		for(int i=0; i<40;i++){
			deletechar();
		}
		printf("%s %u %s, %u:%u:%u", days_of_the_week[dayofweek()], SYSTEM_TIME.day, months_short[SYSTEM_TIME.month], SYSTEM_TIME.hour, SYSTEM_TIME.minute, SYSTEM_TIME.second );
		move_cursor(x, y);

		sleep(100);
	}*/


	//printf("%x\n",info->rsdp);
	//printf("%x\n", info->rsdp->RsdtAddress);
	//busyloop(500000000);
	//detect_cores((void*)(uint64_t)info->rsdp->RsdtAddress);
	//printf("%x\n",info->rsdp);

	//busyloop(500000000);


	//tty tty0=init_tty();


	for (int i = 0; i < 15; ++i){
		fputc('A'+i, stdout);
	}

	fputc(10, stdout);

	fputs("eejfaousdnawndoianwdian",stdout);

	fputs("\n23232323\n",stdout);

	printf("\n23232323\n%f\n",fl);

	printf("\033[s\nhello1\033[uhello2\n");

	uint64_t time=(uint64_t)(TimeSinceBoot*100);

	printf("\033[s\033[30;0H%u %u %u\033[u",time,2,3);

	printf("\033[10;1HHello!\n");

	//loop();

	start_scheduler();
	//nothing after this point should run as the sceduler esentially abandons this base thread
	//maybe this sould be changed to salvage a bit of ram?
	
	return 123;
}
