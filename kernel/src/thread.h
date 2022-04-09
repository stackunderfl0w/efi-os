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
typedef struct {
	uint64_t rdi;
	uint64_t rsi;
	uint64_t rbp;
	uint64_t rsp;
	uint64_t rbx;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rax;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t rip;
	uint64_t rsp0;
	uint64_t cs;
	uint64_t rflags;
	uint64_t cr3;
}registers;

//note, doesnt jump to task but instead replaces return value so next return will have it jump to task
void switch_stack(void *old_ptr, void *new_ptr);
//void push_all();
//void pop_all();

thread* new_thread(void (*function)(void));

/*void swap_context(registers *old_ptr, registers *new_ptr){
	asm volatile(
        "pushfq \n"
        "pushq %%rbx \n"
        "pushq %%rcx \n"
        "pushq %%rbp \n"
        "pushq %%rsi \n"
        "pushq %%rdi \n"
        "pushq %%r8 \n"
        "pushq %%r9 \n"
        "pushq %%r10 \n"
        "pushq %%r11 \n"
        "pushq %%r12 \n"
        "pushq %%r13 \n"
        "pushq %%r14 \n"
        "pushq %%r15 \n"
        "movq %%rsp, %[from_rsp] \n"
        "leaq 1f(%%rip), %%rbx \n"
        "movq %%rbx, %[from_rip] \n"
        "movq %[to_rsp0], %%rbx \n"
        "movl %%ebx, %[tss_rsp0l] \n"
        "shrq $32, %%rbx \n"
        "movl %%ebx, %[tss_rsp0h] \n"
        "movq %[to_rsp], %%rsp \n"
        "pushq %[to_thread] \n"
        "pushq %[from_thread] \n"
        "pushq %[to_rip] \n"
        "cld \n"
        "movq 16(%%rsp), %%rsi \n"
        "movq 8(%%rsp), %%rdi \n"
        "jmp enter_thread_context \n"
        "1: \n"
        "popq %%rdx \n"
        "popq %%rax \n"
        "popq %%r15 \n"
        "popq %%r14 \n"
        "popq %%r13 \n"
        "popq %%r12 \n"
        "popq %%r11 \n"
        "popq %%r10 \n"
        "popq %%r9 \n"
        "popq %%r8 \n"
        "popq %%rdi \n"
        "popq %%rsi \n"
        "popq %%rbp \n"
        "popq %%rcx \n"
        "popq %%rbx \n"
        "popfq \n"
        : [from_rsp] "=m" (old_ptr->rsp),
        [from_rip] "=m" (old_ptr->rip),
        [tss_rsp0l] "=m" (m_tss.rsp0l),
        [tss_rsp0h] "=m" (m_tss.rsp0h),
        "=d" (old_ptr), // needed so that from_thread retains the correct value
        "=a" (new_ptr) // needed so that to_thread retains the correct value
        : [to_rsp] "g" (new_ptr->regs().rsp),
        [to_rsp0] "g" (new_ptr->regs().rsp0),
        [to_rip] "c" (new_ptr->regs().rip),
        [old_ptr] "d" (old_ptr),
        [new_ptr] "a" (new_ptr)
        : "memory", "rbx"
    );
    // clang-format on

    //dbgln_if(CONTEXT_SWITCH_DEBUG, "switch_context <-- from {} {} to {} {}", VirtualAddress(from_thread), *from_thread, VirtualAddress(to_thread), *to_thread);
}
*/