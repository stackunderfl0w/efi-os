#include "interrupt_handlers.h"
#include "keyboard.h"
#include "io.h"
#include "pit.h"
#include "mouse.h"
#include "scheduler.h"
#include "graphics.h"
#include "loop.h"
#include "stdio.h"
extern graphics_context* k_context;

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
	//Panic("Page Fault Detected");
	move_cursor(k_context,60,0);
	for (int i = 0; i < 19; ++i)
	{
		deletechar(k_context);
	}
	print(k_context,"PAGE FAULT DETECTED");

	uint64_t err;
	asm ("mov %%cr2, %0" : : "r" (err));
	printf("%p\n",err);
	loop();
}
__attribute__((interrupt)) void GeneralFault_Handler(struct interrupt_frame* frame){
	move_cursor(k_context,29,0);
	for (int i = 0; i < 19; ++i)
	{
		deletechar(k_context);
	}
	print(k_context,"GENERAL FAULT DETECTED");
	//while(1);
	loop();
}
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame){
	move_cursor(k_context,29,0);
	for (int i = 0; i < 19; ++i)
	{
		deletechar(k_context);
	}
	print(k_context,"Double FAULT DETECTED");
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

__attribute__((interrupt)) void PIT_Handler(struct interrupt_frame* frame){
	//print("keyboard");
	//printchar('t');
	PIT_TICK();
	PIC_EndMaster();
	//handle_scheduler();
	printf("\nip %u\ncs %u\nflags %u\nsp %u\nss %u\nerr %u\npad %u\npad2 %u",frame->ip,frame->cs,frame->flags,frame->sp,frame->ss,frame->err,frame->pad,frame->pad2);
	asm volatile("cli");
	loop();
}
