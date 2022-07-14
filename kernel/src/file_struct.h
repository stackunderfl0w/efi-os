#pragma once
#include "stdbool.h"
typedef struct FILE FILE;

struct FILE{
	int flags;//unused
	char* base;
	char* end;
	char* read_head;
	char* write_head;

	void (*write)(FILE*, char);
	char (*read)(FILE*); 
	void (*sync)(FILE*); 


	_Atomic volatile bool io_lock;
};