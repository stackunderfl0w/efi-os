#pragma once
#include "graphics.h"


void start_scheduler();

void handle_scheduler();

void* get_next_thread();

void new_process(char* executable,void* ptr);
