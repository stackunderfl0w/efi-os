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

void tty_loop(tty this);