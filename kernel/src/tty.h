#pragma once
#include "stdio.h"
#include "file_struct.h"
#include "graphics.h"
typedef struct {
	Framebuffer* buf;
	bitmap_font* console_font;
	//FILE* stdout;
}tty; 

void tty_sync(FILE* f);
void tty_write(FILE* f, char c);
char tty_read(FILE* f);


tty init_tty();
tty init_tty_0(FILE* stdout_0, FILE* stdin_0, char* stdout_buf, char* stdin_buf, unsigned buf_size);

void tty_loop(tty tt);