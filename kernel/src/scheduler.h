#pragma once
#include "typedef.h"
extern bool scheduler_inited;
extern void yield();


void start_scheduler();

void handle_scheduler();

void* get_next_thread();

void new_process(char* executable,void* ptr);
