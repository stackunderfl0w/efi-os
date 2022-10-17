#include "interrupt_handlers.h"
#include "keyboard.h"
#include "io.h"
#include "pit.h"
#include "mouse.h"
#include "scheduler.h"
#include "graphics.h"
#include "loop.h"
#include "stdio.h"
extern graphics_context* global_context;
char buf[128];
uint64_t cr0, cr2, cr3, err;

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
	//Panic("Page Fault Detected");
	move_cursor(global_context,60,0);
	for (int i = 0; i < 19; ++i){
		deletechar(global_context);
	}
	print(global_context,"PAGE FAULT DETECTED");

    asm volatile(
        "mov %%cr0, %%rax\n\t"
        "mov %%eax, %0\n\t"
        "mov %%cr2, %%rax\n\t"
        "mov %%eax, %1\n\t"
        "mov %%cr3, %%rax\n\t"
        "mov %%eax, %2\n\t"
        "mov (%%rsp), %%rax\n\t"
        "mov %%eax, %2\n\t"
    : "=m" (cr0), "=m" (cr2), "=m" (cr3), "=m" (err)
    : /* no input */
    : "%rax"
    );
	//printf("%p\n",err);
	sprintf(buf,"%p",cr2);
	print(global_context,buf);
	loop();
}
__attribute__((interrupt)) void GeneralFault_Handler(struct interrupt_frame* frame){
	move_cursor(global_context,29,0);
	for (int i = 0; i < 19; ++i){
		deletechar(global_context);
	}
	print(global_context,"GENERAL FAULT DETECTED");
	//while(1);
	loop();
}
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame){
	move_cursor(global_context,29,0);
	for (int i = 0; i < 19; ++i){
		deletechar(global_context);
	}
	print(global_context,"Double FAULT DETECTED");
	//while(1);
	loop();
}
__attribute__((interrupt)) void Keyboard_Handler(struct interrupt_frame* frame){
	//print("keyboard");
	uint8_t keycode =inb(0x60);
	handle_key(keycode);
	PIC_EndMaster();
}
__attribute__((interrupt)) void Mouse_Handler(struct interrupt_frame* frame){
	uint8_t keycode =inb(0x60);
	HANDLE_PS2_MOUSE(keycode);
	PIC_EndSlave();
}

__attribute__((interrupt)) void test_handler(struct interrupt_frame* frame){
	printf("test signal recieved");
}

/*__attribute__((interrupt)) void PIT_Handler(struct interrupt_frame* frame){
	//print("keyboard");
	//printchar('t');
	PIT_TICK();
	PIC_EndMaster();
	//handle_scheduler();
	printf("\nip %u\ncs %u\nflags %u\nsp %u\nss %u\nerr %u\npad %u\npad2 %u",frame->ip,frame->cs,frame->flags,frame->sp,frame->ss,frame->err,frame->pad,frame->pad2);
	asm volatile("cli");
	loop();
}*/