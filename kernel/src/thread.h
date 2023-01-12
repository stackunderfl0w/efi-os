#pragma once
#include "typedef.h"
#include "file_table.h"

typedef struct{
	uint64_t tid;
	uint64_t parent_pid;
	void* RSP;
	uint64_t state;
}thread;

#define THREAD_DEAD 1<<0


typedef struct {
	uint64_t rax;//rsp
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbp;
	uint64_t rdi;
	uint64_t rsi;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t rbp0;
	uint64_t ip; //rsp+128
	uint64_t cs; //rsp+136
	uint64_t flags; //rsp+144
	uint64_t sp; //rsp+152
	uint64_t ss; //rsp+160
	uint64_t err;
	uint64_t ret;
}registers;

thread* new_thread(void (*function)(void));
void destroy_thread(thread* th);