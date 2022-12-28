#pragma once
#include "graphics.h"
typedef struct {
	graphics_context* g;
	uint32_t saved_x,saved_y;
}tty; 

void tty_write(tty* tt, char* f);

tty* init_tty();
tty init_tty_0(graphics_context* g);