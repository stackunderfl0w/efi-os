#pragma once
#include <stdarg.h>
#include "bitmap-font.h"
#include "typedef.h"

typedef struct {
	void* BaseAddress;
	uint64_t BufferSize;
	uint32_t Width;
	uint32_t Height;
	uint32_t PixelsPerScanLine;
}Framebuffer;

typedef struct {
	Framebuffer* buf;
	bitmap_font* font;
	UINT32 console_width,console_height;
	UINT32 cursor_x,cursor_y;
}graphics_context;

//void PlotPixel_32bpp(int x, int y, uint32_t pixel);



void render_char(graphics_context* g, UINT32 x, UINT32 y, CHAR8 chr);
void clearchar(graphics_context* g, UINT32 x, UINT32 y);
void deletechar(graphics_context* g);
void printchar(graphics_context* g, char chr);
void print(graphics_context* g, const char* str);

void clrscr(Framebuffer* f, uint32_t color);

graphics_context init_text_overlay(Framebuffer* buf, bitmap_font* font);

void move_cursor(graphics_context* g, UINT32 x, UINT32 y);
void get_cursor_pos(graphics_context* g, UINT32 *x, UINT32 *y);
void cursor_left(graphics_context* g);
void cursor_right(graphics_context* g);
void cursor_up(graphics_context* g);
void cursor_down(graphics_context* g);

void move_mouse(int x, int y);
void draw_mouse(Framebuffer* f);

void get_display_resolution(Framebuffer* f, UINT32 *x, UINT32 *y);
void scroll_console();

void swap_buffer();