[bits 64]
fmt:    db "from asm %x", 10, 0          ; The printf format, "\n",'0'
extern printf
extern loop
extern PIT_TICK
extern PIC_EndMaster
extern get_next_thread

%macro pushall64 0
;stack frame
	push rbp
	mov rbp, rsp
;save all registers
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8

	push rsi
	push rdi
	push rbp
	push rdx
	push rcx
	push rbx
	push rax
%endmacro

%macro popall64 0
;restore registers
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rbp
	pop rdi
	pop rsi

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
%endmacro

Pit_Handler_Asm:
	pushall64
;use pit for time tracking
	call PIT_TICK
	call PIC_EndMaster


;call sceduler and get pointer for next
	;uint64_t ip; rsp+128
	;uint64_t cs; rsp+136
	;uint64_t flags; rsp+144
	;uint64_t sp; rsp+152
	;uint64_t ss; rsp+160
	;uint64_t err;
	;uint64_t pad;
	;uint64_t pad2;
	; rsp+184 is 0xDEADBEEFDEADBEEF end of containing stack
	;so i guess the manual is wrong
	;mov r15,10
	;mov r14,128
;prt:
	;mov	rdi,fmt
	;mov	rsi,[rsp+r14]
	;xor	rax,rax		; or can be  xor  rax,rax
	;call	printf		; Call C function
	;add r14,8
	;dec r15
	;jnz prt
	;call loop
	mov rdi,rsp
	call get_next_thread
	mov rsp,rax


	popall64

;return from interupt
	iretq

global Pit_Handler_Asm

interupt3:
	pushall64
	mov rdi,rsp
	call get_next_thread
	mov rsp,rax
	popall64
	iretq
global interupt3

yield:
	int 3
	mov	rdi,fmt
	mov	rsi,[rsp]
	xor	rax,rax		; or can be  xor  rax,rax
	call	printf		; Call C function
	ret

global yield