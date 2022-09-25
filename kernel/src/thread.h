#pragma once
#include "typedef.h"

#include "vfs.h"
#include "file_table.h"

typedef struct thread thread;

typedef struct{
	uint64_t pid;
	thread* threads[256];
	vfs_node* working_dir;
	file_table* process_file_table;

}process;

struct thread{
	uint64_t tid;
	process* parent_process;
	void* stack_ptr;
	void* RSP;
	uint64_t state;
};


typedef struct {
	uint64_t rax;
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
	uint64_t pad;
	uint64_t pad2;
	uint64_t pad3;
	//adding pad3 somehow makes it work on virtualbox. fuck the intel manual
	//pad 4 then recrashes vb. looks like vb cares about alignment while qemu doesnt.
	//rsp+184 is 0xDEADBEEFDEADBEEF end of containing stack
	//so i guess the manual is wrong but il keep the pading anyways
}registers;

thread* new_thread(void (*function)(void));