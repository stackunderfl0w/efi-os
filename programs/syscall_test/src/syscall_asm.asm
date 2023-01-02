read:
	mov rax,0
	int 0x80
	ret
global read
write:
	mov rax,1
	int 0x80
	ret
global write
open:
	mov rax,2
	int 0x80
	ret
global open

fstat:
	mov rax,4
	int 0x80
	ret
global fstat

kprintf:
	mov rax,99
	int 0x80
	ret
global kprintf

memset:
	mov rax, rdi	;rdi=destination pointer
	mov rcx, rsi	;rsi=value to set
	mov rdx, rdx	;rdx=size
	.memset_loop:	;repeat until rdx = 0

		test rdx, rdx
		jz .memset_exit
		mov [rax], rcx;set next value
		add rax, 1;increment pointer
		dec rdx;decrement counter
		jmp .memset_loop
	.memset_exit:
		ret
global memset