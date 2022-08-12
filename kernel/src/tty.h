#pragma once
#include "stdio.h"
#include "file_struct.h"
#include "graphics.h"
typedef struct {
	Framebuffer* buf;
	bitmap_font* console_font;
	//FILE* stdout;
}tty; 

void sync(FILE* f);
void write(FILE* f, char c);
char read(FILE* f);


tty init_tty();
tty init_tty_0(FILE* stdout_0, char* buf, unsigned buf_size);

void tty_loop(tty tt);