#pragma once
#include "typedef.h"
#include "vfs.h"
#include "file_table.h"

struct thread;

typedef struct{
	uint64_t pid;
	struct thread* threads[256];
	vfs_node* working_dir;
	file_table* process_file_table;
}process;