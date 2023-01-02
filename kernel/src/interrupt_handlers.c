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
uint64_t cr0, cr2, cr3, err;
static inline void get_regs(){
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
} 

/*__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
	//Panic("Page Fault Detected");
	//move_cursor(global_context,60,0);
	//for (int i = 0; i < 19; ++i){
	//	deletechar(global_context);
	//}
	kprintf("PAGE FAULT DETECTED");
	get_regs();
    
	//kprintf("%p\n",err);
	kprintf("%p",cr2);
	//loop();
}*/
/*__attribute__((interrupt)) void GeneralFault_Handler(struct interrupt_frame* frame){
	//move_cursor(global_context,29,0);
	//for (int i = 0; i < 19; ++i){
	//	deletechar(global_context);
	//}
	print(global_context,"GENERAL FAULT DETECTED");
	//while(1);
	loop();
}*/
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
	kprintf("test signal recieved");
}

/*__attribute__((interrupt)) void PIT_Handler(struct interrupt_frame* frame){
	//print("keyboard");
	//printchar('t');
	PIT_TICK();
	PIC_EndMaster();
	//handle_scheduler();
	kprintf("\nip %u\ncs %u\nflags %u\nsp %u\nss %u\nerr %u\npad %u\npad2 %u",frame->ip,frame->cs,frame->flags,frame->sp,frame->ss,frame->err,frame->pad,frame->pad2);
	asm volatile("cli");
	loop();
}*/