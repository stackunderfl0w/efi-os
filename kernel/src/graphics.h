#pragma once
#include <stdarg.h>
#include "bitmap-font.h"
#include "typedef.h"
#include "pit.h"
#include "serial.h"

typedef struct {
	void* BaseAddress;
	uint64_t BufferSize;
	uint32_t Width;
	uint32_t Height;
	uint32_t PixelsPerScanLine;
}Framebuffer;

void PlotPixel_32bpp(int x, int y, uint32_t pixel);


char* to_string(size_t x);

char* to_hstring(size_t x);
char* to_hstring_noformat(size_t x);
char* to_string_double(double x, uint64_t precision);
long atoi(const char* buf);

void putchar(UINT32 x, UINT32 y, CHAR8 chr);
void clearchar(UINT32 x, UINT32 y);
void deletechar();
void printchar(char chr);
void print(const char* str);
void printf(const char* str, ...);

void clrscr(uint32_t color);

void init_text_overlay(Framebuffer* buf, bitmap_font* font);

void move_cursor(UINT32 x, UINT32 y);
void get_cursor_pos(UINT32 *x, UINT32 *y);
void cursor_left();
void cursor_right();
void cursor_up();
void cursor_down();

void move_mouse(int x, int y);
void draw_mouse();
void clear_mouse();

void get_display_resolution(UINT32 *x, UINT32 *y);
void scroll_console();
