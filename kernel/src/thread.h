#pragma once
#include "typedef.h"
typedef struct {
	uint64_t tid;
	void *stack_ptr;
	uint64_t state;
}thread;
typedef struct {
	uint64_t RBP;
	uint64_t RBX;
	uint64_t R12;
	uint64_t R13;
	uint64_t R14;
	uint64_t R15;
	uint64_t RBP2;
	uint64_t ret;
}swtch_stack;


void switch_stack(void *old_ptr, void *new_ptr);
thread* new_thread(void (*function)(void));