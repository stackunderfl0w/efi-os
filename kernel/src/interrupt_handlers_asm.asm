[bits 64]
fmt:    db "from asm %x", 10, 0          ; The printf format, "\n",'0'
extern printf
extern loop
extern PIT_TICK
extern PIC_EndMaster

Pit_Handler_Asm:
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

	mov	rdi,fmt
	mov	rsi,[rsp+128]
	xor	rax,rax		; or can be  xor  rax,rax
	;call	printf		; Call C function



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

	;mov	rdi,fmt
	;mov	rsi,rsp
	;mov	rax,0		; or can be  xor  rax,rax
	;mov rbx,9
	;call    printf		; Call C function
	;cli
;jop:
	;inc r15
	;push r15
	;call    printf		; Call C function
	;pop r15
	;cmp r15,100000000
	;jmp jop
;return from interupt
	iretq

global Pit_Handler_Asm