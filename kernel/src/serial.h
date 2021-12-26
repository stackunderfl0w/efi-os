#pragma once
#include "io.h"
#define PORT 0x3f8          // COM1
 
int init_serial();
int is_transmit_empty();
 
void write_serial(char a);

void print_serial(char* str);