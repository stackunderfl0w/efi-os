#include "thread.h"
#include "paging.h"
#define incptr(p, n) ((void *)(((uintptr_t)(p)) + (n)))


uint64_t next_tid = 1;

thread* new_thread(void (*function)(void)){
	thread *th = REQUEST_PAGE();
	th->tid = next_tid++;
	th->stack_ptr = incptr(th, 4096 - sizeof(swtch_stack));

	swtch_stack *stk = th->stack_ptr;
	stk->RBP = (uint64_t)&stk->RBP2;
	stk->ret = (uint64_t)function;

	return th;
}