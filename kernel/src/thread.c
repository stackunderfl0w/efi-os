#include "thread.h"
#include "paging.h"
#include "stdio.h"
#include "loop.h"
#include "scheduler.h"

extern graphics_context* global_context;
extern graphics_context* k_context;

uint64_t next_tid = 1;

thread* current_thread;

void thread_exit(){
	kprintf("thread returned\n");
	current_thread->state=THREAD_DEAD;
	//should be destroyed after 1st yield, but just in case
	while(1)yield();
}

thread* new_thread(void (*function)(void*),void* arg){
	//todo map memory to new adress space
	thread *th = REQUEST_PAGE();
	th->tid = next_tid++;
	th->state=0;
	th->RSP = incptr(th, 4096 - sizeof(registers));

	registers *reg = th->RSP;

	//set up interrupt frame, the part that actually matters. All registers are included but we only set the ones that matter
	reg->rcx=0xdeadbeefbeadbeef;//debug
	reg->ip = (uint64_t)function;
	reg->cs=0x8;
	reg->sp=(uint64_t)&reg->ret;
	reg->ss=0x10;
	reg->flags=0x202;//no reason
	reg->err=0;
	reg->rbp=(uint64_t)&reg->rbp0;
	reg->ret=(uint64_t)thread_exit;

	return th;
}
void destroy_thread(thread* th){
	FREE_PAGE(th);
}