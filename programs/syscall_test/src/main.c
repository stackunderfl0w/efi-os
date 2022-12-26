//#include "graphics.h"
#include <stdbool.h>
unsigned char x=0;
extern long int syscall(int call_number, ...);

extern long int read(int fd, void *buf, long int count);
extern long int write(int fd, void *buf, long int count);
extern long int open(char*,int);
extern long int kprintf(const char* s, ...);



int _start(char* buf){

	int f=open("/resources/resourcesresources/config.txt",0);
	char txt[16]={0};
	int x=read(f,txt,15);
	kprintf("sup read %d chars: %s\n",x,txt);


	while(1){
		asm volatile("int $3");
	}
	return 0;
}