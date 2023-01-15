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
#include "fat.h"
#include "thread.h"
#include "scheduler.h"
#include "time.h"
#include "shell.h"
#include "stdio.h"
#include "smp.h"
#include "tty.h"
#include "filesystem.h"
#include "process.h"
#include "loop.h"

typedef struct {
	Framebuffer* buf;
	bitmap_font* font;
	EFI_MEMORY_DESCRIPTOR* mem_map;
	UINTN map_size;
	UINTN map_desc_size;
	RSDPDescriptor* rsdp;
}bootinfo;


graphics_context* global_context;
graphics_context* current_context;

process* current_process;
tty tty0;
//#define DISSABLE_FB_BUFFER 1

int _start(bootinfo *info){
	asm("cli");
	//init_serial();
	//print_serial("hello world");

	graphics_context global_graphics=init_text_overlay(info->buf, info->font);

	global_context=current_context=&global_graphics;

	tty0=init_tty_0(&global_graphics);

	kprintf("Bootloader exited successfully\n");
	kprintf("Current stack at about %p\n",tty0);
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

	kprintf("Framebuffer base:%u\n",(uint64_t)info->buf->BaseAddress);

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
	mkdir("TESTDIR",0);

    print_vfs_recursive(root,0);

	#ifndef DISSABLE_FB_BUFFER
		void* new_fb=(void*)0x7000000000;
		request_mapped_pages(new_fb,info->buf->BufferSize);
		Framebuffer fb=*info->buf;
		fb.BaseAddress=new_fb;
		graphics_context kernel_graphics=init_text_overlay(&fb, info->font);
		kernel_graphics.foreground_color=0x00ffffff;
		kernel_graphics.background_color=0x00000000;
		current_context=&kernel_graphics;
		tty0.g=&kernel_graphics;
	#endif


	kprintf("Enabling interupts\n");

	start_scheduler();
	while(1){
		yield();
	}
	//kprintf("test page allocation\n");
	//sleep(200);
	//uint64_t* test_loc=(uint64_t*)0x40000000;

	//*test_loc=90194102;

	//kprintf("survived\n");
	




	//new_process("/resources/syscall_test.elf", current_context->buf->BaseAddress);

	while(1){
		yield();
	}

	
	//kprintf("%x\n",info->rsdp);
	//kprintf("%x\n", info->rsdp->RsdtAddress);
	//busyloop(500000000);
	//detect_cores((void*)(uint64_t)info->rsdp->RsdtAddress);
	//kprintf("%x\n",info->rsdp);

	run_shell(info->buf, info->font);


	loop();	
	return 123;
}
