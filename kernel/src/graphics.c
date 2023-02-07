#include "graphics.h"
#include "stdlib.h"
#include "serial.h"
#include "paging.h"
#include "memory.h"

static inline void PlotPixel_32bpp(Framebuffer* f, int x, int y, uint32_t pixel){
	*((uint32_t*)((char*)f->BaseAddress + 4 * f->PixelsPerScanLine * y + 4 * x)) = pixel;
}
static inline uint32_t ReadPixel_32bpp(Framebuffer* f,int x, int y){
	return *(uint32_t*)(f->BaseAddress + 4 * f->PixelsPerScanLine * y + 4 * x);
}

void render_char(graphics_context* g, UINT32 x, UINT32 y, CHAR8 chr){
	uint32_t x_vis=MIN(g->font->width,g->buf->Width-x);
	uint32_t y_vis=MIN(g->font->height,g->buf->Height-y);

	for (uint32_t y2 = 0; y2 < y_vis; ++y2){
		for (uint32_t x2 = 0; x2 < x_vis; ++x2){
			if ((g->font->buffer[g->font->bytesperglyph*chr+(y2*((g->font->width+7)/8))+x2/8]>>(7-(x2%8)))&0x1){
				PlotPixel_32bpp(g->buf,x+x2,y+y2,g->foreground_color);
			}
			else{
				PlotPixel_32bpp(g->buf,x+x2,y+y2,g->background_color);
			}
		}
	}
}

void clearchar(graphics_context* g,UINT32 x, UINT32 y){
	for (uint32_t y2 = 0; y2 < g->font->height; ++y2){
		for (uint32_t x2 = 0; x2 < g->font->width; ++x2){
			PlotPixel_32bpp(g->buf,x+x2,y+y2,0x00000000);
		}
	}
}

void deletechar(graphics_context* g){
	cursor_left(g,1);
	clearchar(g,g->cursor_x*g->font->width, g->cursor_y*g->font->height);
}

void printchar(graphics_context* g, char chr){
	if (chr=='\n'){
		g->cursor_x=0;
		g->cursor_y++;
		if(g->cursor_y>=g->console_height){
			g->cursor_y--;
			scroll_console(g);       
		}
		return;
	}
	if(chr=='\t'){
		g->cursor_x=(g->cursor_x+4)&~0x3;
		return;
	}
	if(g->cursor_y>=g->console_height){
		g->cursor_y=g->console_height-1;
		scroll_console(g);       
	}
	//save and calculate new position before printing char for multithreading
	int print_x=g->cursor_x,print_y=g->cursor_y;
	g->cursor_x++;
	if (g->cursor_x>=g->console_width){
		g->cursor_x=0;
		g->cursor_y++;
	}
	render_char(g,print_x*g->font->width, print_y*g->font->height, chr);
}
void print(graphics_context* g, const char* str){
	//print_serial(str);
	for (int i = 0; str[i]!=0; ++i){
		printchar(g, str[i]);
	}
}

void clrscr(Framebuffer* f, uint32_t color){
	uint64_t flags=enter_critical();
	uint64_t end=f->BufferSize/4;
	for (int i = 0; i < end; ++i){
		((uint32_t*)f->BaseAddress)[i]=color;
	}
	exit_critical(flags);
}

graphics_context init_text_overlay(Framebuffer* buf, bitmap_font* font){
	graphics_context g={buf,font,(buf->Width/font->width),(buf->Height/font->height),0,0,0xffffffff,0x00000000};
	clrscr((&g)->buf,0x12121212);
	return g;
}
Framebuffer* alloc_framebuffer(uint32_t width, uint32_t height, void* addr){
	request_mapped_pages(addr,width*height*4);

	Framebuffer* fb=kmalloc(sizeof(Framebuffer));
	*fb=(Framebuffer){.BaseAddress=addr,.BufferSize=width*height*4,.Width=width,.Height=height,.PixelsPerScanLine=width};
	return fb;
}

void move_cursor(graphics_context* g, UINT32 x, UINT32 y){
	g->cursor_x=x;
	g->cursor_y=y;
}

void get_cursor_pos(graphics_context* g, UINT32 *x, UINT32 *y){
	*x=g->cursor_x;
	*y=g->cursor_y;
}

void cursor_left(graphics_context* g, uint64_t distance){
	g->cursor_x=MAX(g->cursor_x,1)-1;//prevent underflow
}
void cursor_right(graphics_context* g, uint64_t distance){
	g->cursor_x=MIN(g->cursor_x+1,g->console_width);
}
void cursor_up(graphics_context* g, uint64_t distance){
	g->cursor_y=MAX(g->cursor_y,1)-1;//prevent underflow
}
void cursor_down(graphics_context* g, uint64_t distance){
	g->cursor_y=MIN(g->cursor_y+1,g->console_height);
}

uint16_t cursor[16]={
	0b1000000000000000,
	0b1100000000000000,
	0b1110000000000000,
	0b1111000000000000,
	0b1111100000000000,
	0b1111110000000000,
	0b1111111000000000,
	0b1111111100000000,
	0b1111111110000000,
	0b1111111111000000,
	0b1111111111100000,
	0b1111110000000000,
	0b1111111000000000,
	0b1110111100000000,
	0b1100011110000000,
	0b1000001100000000
};

int mouse_cursor_x=0, mouse_cursor_y=0;
void move_mouse(int x, int y){
	mouse_cursor_x=x;
	mouse_cursor_y=y;
}

void draw_mouse(Framebuffer* f){
	int mx=mouse_cursor_x,my=mouse_cursor_y;
	uint8_t x_vis=MIN(16,f->Width-mx);
	uint8_t y_vis=MIN(16,f->Height-my);

	for (int y = 0; y < y_vis; ++y){
		for (int i = 0; i < x_vis; ++i){
			if(cursor[y]>>(15-i)&0x1){
				PlotPixel_32bpp(f,mx+i,my+y,0xffffffff);
			}
		}
	}
}

void get_display_resolution(Framebuffer* f, UINT32 *x, UINT32 *y){
	*x=f->Width;
	*y=f->Height;
}

void scroll_console(graphics_context* g){
	uint64_t flags=enter_critical();
	int row_size_px=g->buf->PixelsPerScanLine*g->font->height;
	//move everything up 1 line
	memcpy(g->buf->BaseAddress, g->buf->BaseAddress+(row_size_px)*4, row_size_px*(g->console_height-1)*4);
	//clear bottom line
	memset(g->buf->BaseAddress+row_size_px*(g->console_height-1)*4, 0, g->buf->BufferSize-(4*row_size_px*(g->console_height-1)));
	exit_critical(flags);
}

void swap_buffer(Framebuffer* dest,Framebuffer* src){
	//swap fb in 2 chunks to render mouse while swapping to avoid flickering
	uint64_t* midpoint=(dest->BaseAddress+dest->PixelsPerScanLine*MIN(mouse_cursor_y+16,dest->Height)*4);
	uint64_t* end=(dest->BaseAddress+dest->BufferSize);

	uint64_t* dst=dest->BaseAddress;
	uint64_t* sc=src->BaseAddress;

	for (; dst < midpoint;){
		*dst++=*sc++;
	}
	draw_mouse(dest);
	for (; dst < end;){
		*dst++=*sc++;
	}
	//not faster somehow
	//uint64_t n=((uint64_t)end-(uint64_t)midpoint)/8;
	/*__asm__ volatile(
    "mov %0, %%rdi;"
    "mov %1, %%rsi;"
    "mov %2, %%rcx;"
    "rep movsq;"
    :
    : "r"(dst), "r"(sc), "r"(n)
    : "rdi", "rsi", "rcx"
  	);*/
}