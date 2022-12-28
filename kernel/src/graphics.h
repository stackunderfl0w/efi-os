#pragma once
#include "typedef.h"
#include "bitmap-font.h"

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
	uint32_t console_width,console_height;
	uint32_t cursor_x,cursor_y;
	uint32_t foreground_color;
	uint32_t background_color;
}graphics_context;

//void PlotPixel_32bpp(int x, int y, uint32_t pixel);



void render_char(graphics_context* g, uint32_t x, uint32_t y, CHAR8 chr);
void clearchar(graphics_context* g, uint32_t x, uint32_t y);
void deletechar(graphics_context* g);
void printchar(graphics_context* g, char chr);
void print(graphics_context* g, const char* str);

void clrscr(Framebuffer* f, uint32_t color);

graphics_context init_text_overlay(Framebuffer* buf, bitmap_font* font);

void move_cursor(graphics_context* g, uint32_t x, uint32_t y);
void get_cursor_pos(graphics_context* g, uint32_t *x, uint32_t *y);
void cursor_left(graphics_context* g, uint64_t distance);
void cursor_right(graphics_context* g, uint64_t distance);
void cursor_up(graphics_context* g, uint64_t distance);
void cursor_down(graphics_context* g, uint64_t distance);

void move_mouse(int x, int y);
void draw_mouse(Framebuffer* f);

void get_display_resolution(Framebuffer* f, uint32_t *x, uint32_t *y);
void scroll_console();

void swap_buffer();