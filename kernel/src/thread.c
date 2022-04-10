#include "thread.h"
#include "paging.h"
#define incptr(p, n) ((void *)(((uintptr_t)(p)) + (n)))


uint64_t next_tid = 1;

thread* new_thread_old(void (*function)(void)){
	thread *th = REQUEST_PAGE();
	th->tid = next_tid++;
	th->stack_ptr = incptr(th, 4096 - sizeof(swtch_stack));

	swtch_stack *stk = th->stack_ptr;
	stk->RBP = (uint64_t)&stk->RBP2;
	stk->ret = (uint64_t)function;

	return th;
}
thread* new_thread(void (*function)(void)){
	thread *th = REQUEST_PAGE();
	th->tid = next_tid++;
	th->stack_ptr = incptr(th, 4096 - sizeof(registers));

	registers *reg = th->stack_ptr;

	//set up interrupt frame, the part that actually matters
	reg->rcx=0xdeadbeefbeadbeef;
	reg->ip = (uint64_t)function;
	reg->cs=0x8;
	reg->sp=(uint64_t)th->stack_ptr;
	reg->ss=0x10;
	reg->flags=0x202;//no reason
	reg->err=0;
	reg->pad=0;
	reg->rbp=(uint64_t)&reg->rbp0;

	th->RSP=th->stack_ptr;

	return th;
}