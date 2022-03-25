#pragma once
#include <stdarg.h>
#include "typedef.h"

typedef struct {
	void* BaseAddress;
	uint64_t BufferSize;
	uint32_t Width;
	uint32_t Height;
	uint32_t PixelsPerScanLine;
}Framebuffer;

void PlotPixel_32bpp(Framebuffer *buf, int x, int y, uint32_t pixel){
	*((uint32_t*)((char*)buf->BaseAddress + 4 * buf->PixelsPerScanLine * y + 4 * x)) = pixel;
}

void clrscr(Framebuffer *buf, uint32_t color){
	uint64_t flags;
	asm volatile("# __raw_save_flags\n\t"
		"pushf ; pop %0"
		: "=rm" (flags)
		: /* no input */
		: "memory");
	if (flags&0x200){
		asm volatile("cli");
	}
	uint64_t end=buf->BufferSize/4;
	for (int i = 0; i < end; ++i){
		((uint32_t*)buf->BaseAddress)[i]=color;
	}
	asm volatile("sti");
}
void get_display_resolution(UINT32 *x, UINT32 *y);
void scroll_console();
