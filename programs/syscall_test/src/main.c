//#include "graphics.h"
#include <stdbool.h>
unsigned char x=0;
typedef unsigned long long uint64_t;
struct stat{
	uint64_t	st_dev;         /* ID of device containing file */
	uint64_t	st_ino;         /* Inode number */
	uint64_t	st_mode;        /* File type and mode */
	uint64_t	st_nlink;       /* Number of hard links */
	uint64_t	st_uid;         /* User ID of owner */
	uint64_t	st_gid;         /* Group ID of owner */
	uint64_t	st_rdev;        /* Device ID (if special file) */
	uint64_t	st_size;        /* Total size, in bytes */
	uint64_t	st_blksize;     /* Block size for filesystem I/O */
	uint64_t	st_blocks;      /* Number of 512B blocks allocated */
};

extern long int read(int fd, void *buf, long int count);
extern long int write(int fd, void *buf, long int count);
extern long int open(char*,int);
extern long int fstat(int fd, struct stat *buf);
extern long int kprintf(const char* s, ...);



int _start(char* buf){

	int f=open("/resources/resourcesresources/config.txt",0);
	struct stat stat1;

	fstat(f,&stat1);
	char txt[256]={0};
	int len=stat1.st_size;
	if(len>255){
		len=255;
	}
	int x=read(f,txt,len);
	//kprintf("sup read %d chars: %s\n",x,txt);

	int tout=open("/dev/tty1/tty_out",0);

	write(tout,txt,x);



	while(1){
		//write(tout,"hello",6);
		asm volatile("int $3");
	}
	return 0;
}