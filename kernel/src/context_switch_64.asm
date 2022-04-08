[bits 64]
switch_stack_old:
	push rbp
	mov rbp, rsp

	push r15
	push r14
	push r13
	push r12
	push rbx
	push rbp

	mov [rdi], rsp
	mov rsp, [rsi]

	pop rbp
	pop rbx
	pop r12
	pop r13
	pop r14
	pop r15

	mov rsp, rbp
	pop rbp

	ret
GLOBAL switch_stack_old

switch_stack:
	push rbp
	mov rbp, rsp

	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8

	push rdx
	push rcx
	push rbx
	push rax
	push rbp

	mov [rdi], rsp
	mov rsp, [rsi]

	pop rbp
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15

	mov rsp, rbp
	pop rbp

	ret
GLOBAL switch_stack

push_all:
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8

	push rdx
	push rcx
	push rbx
	push rax
	push rbp

	ret
GLOBAL push_all

pop_all:
	pop rbp
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15

	ret
GLOBAL pop_all
