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
#include "thread.h"
#include "scheduler.h"
#include "time.h"
#include "keyboard.h"
#include "shell.h"
#include "stdio.h"
#include "smp.h"
#include "tty.h"
#include "filesystem.h"
#include "process.h"

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

process* current_process;


int _start(bootinfo *info){
	asm("cli");
	//init_serial();
	//print_serial("hello world");

	//manually create kernel stdout&tty so it can be created without heap and remains in scope
	graphics_context global_graphics=init_text_overlay(info->buf, info->font);
	k_context=global_context=&global_graphics;

	//the boot thread has a minimum stack size of 128k as specified in the uefi standard. 
	//Might as well make use of that with some stack allocations
	FILE stdout_0;
	FILE stdin_0;
	char stdout_buf[8192];
	char stdin_buf[8192];

	tty tty0=init_tty_0(&stdout_0,&stdin_0,stdout_buf,stdin_buf,8192);

	kprintf("Bootloader exited successfully\n");
	kprintf("Current stack at about %p\n",stdout_buf);
	kprintf("_start located at %p\n",_start);
	//loop();

	INIT_GDT();
	kprintf("GDT loaded\n");
	
	create_interrupts();
	kprintf("Interupts loaded\n");

	//SET_PIT_DIVISOR(65535);
	SET_PIT_FREQUENCY(1000);
	kprintf("Pit set\n");

	INIT_RTC();
	kprintf("RTC date&time read");

	INIT_PS2_MOUSE();
	kprintf("PS2 mouse initialized\n");

	kprintf("Frambuffer base:%u\n",(uint64_t)info->buf->BaseAddress);

	uint32_t numEntries=info->map_size/info->map_desc_size;
	uint64_t memsize=getMemorySize(info->mem_map,numEntries,info->map_desc_size);

	kprintf("Total memory:%u(%uMB)\n",memsize,memsize/(1024*1024));

	INIT_PAGING(info->mem_map,numEntries,info->map_desc_size,info->buf);
	kprintf("Paging initialized\n");

	kprintf("Free memory: %ukb\n",get_free_memory()/1024);
	kprintf("Used memory: %ukb\n",get_used_memory()/1024);
	kprintf("Reserved memory: %ukb\n",get_reserved_memory()/1024);

	void* kernel_heap=(void*)0x80000000000;
	INIT_HEAP(kernel_heap,0x10);
	kprintf("Kernel Heap initialized at %p\n",kernel_heap);
	
	INIT_FILESYSTEM();
	kprintf("Fat file system loaded\n");
	file_table* placeholder=init_file_table();

	vfs_node* root= vfs_create_root(0);
	kprintf("Virtual file system starting up\n");

	process p;
	current_process=&p;
	current_process->working_dir=root;
	current_process->process_file_table=placeholder;

	vfs_recursive_populate(root,"/",5);
	kprintf("Virtual file system populated\n");

	print_vfs_recursive(root,0);

	//detect_cores((void*)(uint64_t)info->rsdp->RsdtAddress);
	//loop();

	void* new_fb=(void*)0x7000000000;
	request_mapped_pages(new_fb,info->buf->BufferSize);
	Framebuffer fb=*info->buf;
	fb.BaseAddress=new_fb;
	graphics_context kernel_graphics=init_text_overlay(&fb, info->font);
	kernel_graphics.foreground_color=0x00ffffff;
	kernel_graphics.background_color=0x00000000;
	k_context=&kernel_graphics;


	kprintf("Enabling interupts\n");
	swap_buffer(global_context->buf,k_context->buf);
	
	start_scheduler();
	//uint8_t* file = read_file("/resources/startup.txt");
	//kprintf("%s\n",file);



	int f=open("/resources/resourcesresources/config.txt",0);
	//int f2=open("/resources/resourcesresources/config.txt",0);;




	struct stat stat1;

	fstat(f,&stat1);

	kprintf("%u\n",stat1.st_size);


	char* t= kcalloc(stat1.st_size+1);

	read(f,(uint8_t*)t,stat1.st_size);

	t[stat1.st_size]=0;

	kprintf("Before modification: %s\n",t);

	lseek(f,SEEK_CUR,-11);
	write(f,"12345678901234567890",20);
	lseek(f,SEEK_SET,0);
	fstat(f,&stat1);
	char* t2= kmalloc(stat1.st_size+1);
	read(f,t2,stat1.st_size);

	t2[stat1.st_size]=0;
	kprintf("After modification: %s\n",t2);


	close(f);




	new_process("/resources/syscall_test.elf", k_context->buf->BaseAddress);

	while(1){
		yield();
	}
	sleep(1);


	//uint8_t* file = read_file("/resources/TEST    TXT");
	//uint8_t* file = read_file("/resources/startup.txt");

	//uint8_t* font2 = read_file("/resources/zap-light16.psf");

	//write_file("/resources/WRTTEST TXT",file,1024);

	//bitmap_font loaded_font=load_font(font2);

	//sleep(2000);

	//loop();


	
	//kprintf("%x\n",info->rsdp);
	//kprintf("%x\n", info->rsdp->RsdtAddress);
	//busyloop(500000000);
	//detect_cores((void*)(uint64_t)info->rsdp->RsdtAddress);
	//kprintf("%x\n",info->rsdp);



	swap_buffer(global_context->buf,k_context->buf);

	run_shell(info->buf, info->font);


	loop();	
	return 123;
}
