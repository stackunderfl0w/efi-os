//#include "graphics.h"
#include <stdbool.h>
unsigned char x=0;

long int syscall(int call_number, ...){
	long int ret;
    asm volatile(
    	"mov %1,%%edi\n\t"
        "int $0x80\n\t"
        "mov %%rax, %0\n\t"
        : "=m" (ret)
        : "r"(call_number)
        : "rbx",
          "rcx",
          "rdx",
          "rsi",
          "rdi",
          "r8",
          "r9",
          "r10",
          "r11",
          "r12",
          "r13",
          "r14",
          "r15",
          "ymm0",
          "ymm1",
          "ymm2",
          "ymm3",
          "ymm4",
          "ymm5",
          "ymm6",
          "ymm7",
          "ymm8",
          "ymm9",
          "ymm10",
          "ymm11",
          "ymm12",
          "ymm13",
          "ymm14",
          "ymm15"
    );
    return ret;
}


int _start(char* buf){

	syscall(0x69);
	//draw(buf);
	//clrscr(buf, 0x29401930);
	while(1){
		asm volatile("int $3");
	}
	return 0;
}