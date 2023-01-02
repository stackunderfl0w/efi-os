#include "thread.h"
#include "paging.h"
#include "stdio.h"
#include "loop.h"

extern graphics_context* global_context;
extern graphics_context* k_context;

uint64_t next_tid = 1;

void thread_exit(){
	kprintf("thread returned\n");
	loop();
}

thread* new_thread(void (*function)(void)){
	//todo map memory to new adress space
	thread *th = REQUEST_PAGE();
	th->tid = next_tid++;
	th->stack_ptr = incptr(th, 4096 - sizeof(registers));

	registers *reg = th->stack_ptr;

	//set up interrupt frame, the part that actually matters. All registers are included bu we only set the ones that matter
	reg->rcx=0xdeadbeefbeadbeef;//debug
	reg->ip = (uint64_t)function;
	reg->cs=0x8;
	reg->sp=(uint64_t)th->stack_ptr+176;
	reg->ss=0x10;
	reg->flags=0x202;//no reason
	reg->err=0;
	reg->pad=0;
	reg->rbp=(uint64_t)&reg->rbp0;
	reg->err=(uint64_t)0x8888888888;
	reg->pad=(uint64_t)thread_exit;
	reg->pad2=(uint64_t)0x4040404040;
	reg->pad3=(uint64_t)0x4500000505;


	//reg->rdi=(uint64_t)k_context->buf->BaseAddress;

	th->RSP=th->stack_ptr;

	return th;
}