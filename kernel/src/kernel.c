#include "typedef.h"
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
	SET_PIT_FREQUENCY(1000);


	uint32_t numEntries=info->map_size/info->map_desc_size;


	printchar('\n');
	INIT_PS2_MOUSE();

	asm ("sti");

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
	sleep(1000);


	INIT_HEAP((void*)0x8000000000,0x10);



	uint8_t* sector_1=malloc(512);;

	atapio_software_reset(ATAPIO_REGULAR_STATUS_REGISTER_PORT);

	atapio_read_sectors(0, 1, sector_1);

	/*for (int i = 0; i < 512; ++i)
	{
		uint64_t tmp=sector_1[i];
		print(to_hstring_noformat(tmp));
		printchar(' ');
	}*/

	printf("%sknknknknk\n","helloytfoucv");

	INIT_FILESYSTEM();
	
	int file_entries;
	//read_directory("/",&file_entries);
	
	//uint8_t* file = read_file("/resources/TEST    TXT");
	uint8_t* file = read_file("/resources/config.txt");
	//uint8_t* file = read_file("/resources/startup.txt");

	uint8_t* font2 = read_file("/resources/zap-light16.psf");

	bitmap_font loaded_font=load_font(font2);

	init_text_overlay(info->buf, &loaded_font);

	write_file("/resources/WRTTEST TXT",file,1024);

	//file = read_file("/resources/WRTTEST TXT");

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

	run_shell(info->buf, info->font);



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
	}



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
