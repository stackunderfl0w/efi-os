#include "interrupt_handlers.h"
#include "keyboard.h"
#include "io.h"
#include "pit.h"
#include "mouse.h"
#include "scheduler.h"
#include "graphics.h"
#include "loop.h"

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame){
	//Panic("Page Fault Detected");
	move_cursor(60,0);
	for (int i = 0; i < 19; ++i)
	{
		deletechar();
	}
	print("PAGE FAULT DETECTED");

	uint64_t err;
	asm ("mov %%cr2, %0" : : "r" (err));
	print(to_hstring(err));
	while(true){
		asm("hlt");
	}

}
__attribute__((interrupt)) void GeneralFault_Handler(struct interrupt_frame* frame){
	move_cursor(29,0);
	for (int i = 0; i < 19; ++i)
	{
		deletechar();
	}
	print("GENERAL FAULT DETECTED");
	while(1);

}
__attribute__((interrupt)) void DoubleFault_Handler(struct interrupt_frame* frame){
	move_cursor(29,0);
	for (int i = 0; i < 19; ++i)
	{
		deletechar();
	}
	print("Double FAULT DETECTED");
	while(1);

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
