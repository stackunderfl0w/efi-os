#pragma once
#include "stdbool.h"
#include "loop.h"
typedef struct FILE FILE;

enum FILE_FLAGS{
	IO_UNBUFFERED=1<<0,
	IO_LINE_BUFFERED=1<<1,
	IO_FULLY_BUFFERED=1<<2,
	IO_NO_SYNC=1<<2,
};

struct FILE{
	int flags;
	char* base;
	char* end;
	char* read_head;
	char* write_head;

	void (*write)(FILE*, char);
	char (*read)(FILE*);
	void (*sync)(FILE*);
	void (*overflow)(FILE*);

	_Atomic volatile bool io_lock;
};

static inline void aquire_lock(_Atomic volatile bool *lock){
	while(*lock)
		busyloop(1);
	*lock=true;
}
static inline void release_lock(_Atomic volatile bool *lock){
	*lock=false;
}