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
graphics_context* global_context;


int _start(bootinfo *info){
	asm("cli");

	//init_serial();
	//print_serial("hello world");

	//manually create kernel stdout&tty so it can be created without heap and remains in scope
	graphics_context global_graphics=init_text_overlay(info->buf, info->font);
	k_context=global_context=&global_graphics;

	//the boot thread has a minimum stack size of 128k as specified in the uefi standard. 
	//Might as well makde use of that with some stack allocations
	FILE stdout_0;
	FILE stdin_0;
	char stdout_buf[8192];
	char stdin_buf[8192];

	tty tty0=init_tty_0(&stdout_0,&stdin_0,stdout_buf,stdin_buf,8192);

	printf("Bootloader exited successfully\n");

	INIT_GDT();
	printf("GDT loaded\n");
	
	create_interrupts();
	printf("Interupts loaded\n");

	//SET_PIT_DIVISOR(65535);
	SET_PIT_FREQUENCY(1000);
	printf("Pit set\n");

	INIT_RTC();
	printf("RTC date&time read");

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

	void* kernel_heap=(void*)0x80000000000;
	INIT_HEAP(kernel_heap,0x10);
	printf("Kernel Heap initialized at %p\n",kernel_heap);
	
	INIT_FILESYSTEM();
	printf("Filesystem loaded\n");

	//new_process("/resources/scrclr.elf",info->buf->BaseAddress);


	void* new_fb=(void*)0x7000000000;
	request_mapped_pages(new_fb,info->buf->BufferSize);
	Framebuffer fb=*info->buf;
	fb.BaseAddress=new_fb;
	graphics_context kernel_graphics=init_text_overlay(&fb, info->font);
	kernel_graphics.foreground_color=0x00ff00ff;
	kernel_graphics.background_color=0xff00ff00;
	k_context=&kernel_graphics;


	printf("Enabling interupts");
	swap_buffer(global_context->buf,k_context->buf);

	start_scheduler();

	printf("\n\n\n\n\n");

	sleep(1);


	//uint8_t* file = read_file("/resources/TEST    TXT");
	uint8_t* file = read_file("/resources/startup.txt");

	uint8_t* font2 = read_file("/resources/zap-light16.psf");

	write_file("/resources/WRTTEST TXT",file,1024);

	//bitmap_font loaded_font=load_font(font2);

	//sleep(2000);

	//loop();



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





	swap_buffer(global_context->buf,k_context->buf);

	run_shell(info->buf, info->font);


	double last_frame[60];
	char st[32];
	double fps;
	int count=0;
	while(1){
		//memcpy(&last_frame[1],last_frame,59* sizeof(double));
		for (int i = 59; i > 0; --i){
			last_frame[i]=last_frame[i-1];
		}
		last_frame[0]=TimeSinceBoot;
	
		swap_buffer(global_context->buf,k_context->buf);

		draw_mouse(global_context->buf);
		int x=global_context->cursor_x,y=global_context->cursor_y;
		global_context->cursor_x=10;
		global_context->cursor_y=10;
		count++;
		if (count>10){
			count=0;
			//fps =(1/((last_frame[0]-last_frame[59]))*(60-1));
			fps =1/(last_frame[0]-last_frame[59])*59;
			memset(st,0,32);
			sprintf(st,"%f",fps);
		}
		print(global_context,st);

		sleep(0);
	}
	loop();
	start_scheduler();
	//nothing after this point should run as the sceduler esentially abandons this base thread
	//maybe this sould be changed to salvage a bit of ram?
	
	return 123;
}
