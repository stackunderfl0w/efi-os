#pragma once
#include "typedef.h"
#include "vfs.h"
#include "file_table.h"

typedef struct{
	uint64_t pid;
	uint64_t child_threads[256];
	vfs_node* working_dir;
	file_table* process_file_table;
}process;

void new_process(char* executable,void* ptr);