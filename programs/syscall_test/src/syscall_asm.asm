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

kprintf:
	mov rax,99
	int 0x80
	ret
global kprintf