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

typedef struct {
	Framebuffer* buf;
	bitmap_font* font;
	EFI_MEMORY_DESCRIPTOR* mem_map;
	UINTN map_size;
	UINTN map_desc_size;
}bootinfo;

#ifndef __cplusplus
#include "loop.h"

#else
extern "C" {
#include "loop.h"
}

extern "C" 
#endif

int _start(bootinfo *info){
	asm("cli");
	//init_serial();
	init_text_overlay(info->buf, info->font);
	//print_serial("hello world");

	//gdt
	print("didnt crash ");
	INIT_GDT();
	print("GDT loaded ");

	create_interrupts();


	print("idt loaded ");

	//SET_PIT_DIVISOR(65535);
	SET_PIT_FREQUENCY(100);
	print("pit set ");


	uint32_t numEntries=info->map_size/info->map_desc_size;


	printchar('\n');
	INIT_PS2_MOUSE();

	print("mouse inited");
	print(to_hstring((uint64_t)info->buf->BaseAddress));

	uint64_t memsize=getMemorySize(info->mem_map,numEntries,info->map_desc_size);
	printchar('\n');

	print(to_string(memsize));
	printchar('\n');

	print("Initializing paging\n");
	INIT_PAGING(info->mem_map,numEntries,info->map_desc_size,info->buf);


	print("page table loaded\n");

	printf("Free memory: %ukb\n",get_free_memory()/1024);
	printf("Used memory: %ukb\n",get_used_memory()/1024);
	printf("Reserved memory: %ukb\n",get_reserved_memory()/1024);



	INIT_HEAP((void*)0x8000000000,0x10);
	print("Heap inited\n");


	//asm("sti");
	//sleep(2000);

	/*
	INIT_FILESYSTEM();
	
	int file_entries;
	//read_directory("/",&file_entries);
	
	//uint8_t* file = read_file("/resources/TEST    TXT");
	uint8_t* file = read_file("/resources/startup.nsh");

	uint8_t* font2 = read_file("/resources/zap-light16.psf");

	bitmap_font loaded_font=load_font(font2);

	init_text_overlay(info->buf, &loaded_font);

	write_file("/resources/WRTTEST TXT",file,1024);

	file = read_file("/resources/WRTTEST TXT");

	//printf("\n%s\n",file);
	print(file);
	uint32_t x,y;


	char* chr_ptr=malloc(13);
	char* src="Hello there";
	memcpy(chr_ptr,src,12);
	printf(chr_ptr);
	realloc(chr_ptr,256);
	printf(chr_ptr);

	//printf(chr_ptr);

	INIT_RTC();


	CHAR8* kern =  read_file("/SCRCLR  ELF");

	printf("Kernel loaded , size=%u\n",512);


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

				UINTN size = phdr->p_filesz;
				for (UINTN i = 0; i < size; ++i)
				{
					((char*)segment)[i]=kern[phdr->p_offset+i];
				}
				break;
			}
		}
	}
	printf("Kernel e_entry %u \n",header->e_entry);
	printf("Kernel entry %u \n",&header->e_entry);


	int (*KernelStart)() = ((__attribute__((sysv_abi)) int (*)() ) header->e_entry);

	printf("Kernel start %u \n",&KernelStart);

	//char* nel=calloc(1024);

	//atapio_write_sectors(0,1,file);
	loop();

	run_shell(info->buf, info->font);
*/

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

	//current = new_thread(thread_function);
  	//next = new_thread(thread_function);

 	//uint64_t dummy_stack_ptr;
  	//switch_stack(&dummy_stack_ptr, &current->stack_ptr);
	//asm ("sti");
	start_scheduler();
	while(1){
		clrscr(0xffffffff);
	}

	uint32_t x,y;
	while(1){
		get_cursor_pos(&x, &y);
		move_cursor(20, 0);
		for (int i = 0; i < 10; ++i){
			deletechar();
		}

		uint64_t time=(uint64_t)(TimeSinceBoot*100);
		print(to_string(time));
		printchar(' ');
		print(to_string(x));
		printchar(' ');
		print(to_string(y));

		move_cursor(x, y);
		sleep(50);
	}
	loop();
	return 123;
}
