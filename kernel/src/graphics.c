#include "graphics.h"
#include "stdlib.h"

inline void PlotPixel_32bpp(Framebuffer* f, int x, int y, uint32_t pixel){
	*((uint32_t*)((char*)f->BaseAddress + 4 * f->PixelsPerScanLine * y + 4 * x)) = pixel;
}
inline uint32_t ReadPixel_32bpp(Framebuffer* f,int x, int y){
	return *(uint32_t*)(f->BaseAddress + 4 * f->PixelsPerScanLine * y + 4 * x);
}

uint64_t enter_critical(){
	uint64_t flags;
	asm volatile("# __raw_save_flags\n\t"
		"pushf ; pop %0"
		: "=rm" (flags)
		: /* no input */
		: "memory");
	asm volatile("cli");
	return flags;
}

inline void exit_critical(uint64_t flags){
	if (flags&0x200){
		asm volatile("sti");
	}
}


void putchar(graphics_context* g, UINT32 x, UINT32 y, CHAR8 chr){
	uint32_t x_vis=MIN(g->font->width,g->buf->Width-x);
	uint32_t y_vis=MIN(g->font->height,g->buf->Height-y);

	for (uint32_t y2 = 0; y2 < y_vis; ++y2){
		for (uint32_t x2 = 0; x2 < x_vis; ++x2)
		{
			if ((g->font->buffer[g->font->bytesperglyph*chr+(y2*((g->font->width+7)/8))+x2/8]>>(7-(x2%8)))&0x1){
				PlotPixel_32bpp(g->buf,x+x2,y+y2,0xffffffff);
			}
		}
	}
}

void clearchar(graphics_context* g,UINT32 x, UINT32 y){
	for (uint32_t y2 = 0; y2 < g->font->height; ++y2){
		for (uint32_t x2 = 0; x2 < g->font->width; ++x2)
		{
			PlotPixel_32bpp(g->buf,x+x2,y+y2,0x00000000);
		}
	}
}

void deletechar(graphics_context* g){
	cursor_left(g);
	clearchar(g,g->cursor_x*g->font->width, g->cursor_y*g->font->height);
}

void printchar(graphics_context* g, char chr){
	if (chr=='\n'){
		g->cursor_x=0;
		g->cursor_y++;
		return;
	}
	if(g->cursor_y>=g->console_height){
		g->cursor_y--;
		scroll_console(g);       
	}
	//save and calculate new position before printing char for multithreading
	int print_x=g->cursor_x,print_y=g->cursor_y;
	g->cursor_x++;
	if (g->cursor_x>=g->console_width){
		g->cursor_x=0;
		g->cursor_y++;
	}
	putchar(g,print_x*g->font->width, print_y*g->font->height, chr);
}
void print(graphics_context* g, const char* str){
	//print_serial(str);
	for (int i = 0; str[i]!=0; ++i)
	{
		printchar(g, str[i]);
	}
}




void clrscr(Framebuffer* f, uint32_t color){
	//int end=globalBuf->PixelsPerScanLine*globalBuf->Height;
	uint64_t flags;
	asm volatile("# __raw_save_flags\n\t"
		"pushf ; pop %0"
		: "=rm" (flags)
		: /* no input */
		: "memory");
	if (flags&0x200){
		asm volatile("cli");
	}
	uint64_t end=f->BufferSize/4;
	for (int i = 0; i < end; ++i){
		((uint32_t*)f->BaseAddress)[i]=color;
	}
	if (flags&0x200){
		asm volatile("sti");
	}
}

graphics_context init_text_overlay(Framebuffer* buf, bitmap_font* font){
	graphics_context g={buf,font,(buf->Width/font->width),(buf->Height/font->height),0,0};
	clrscr((&g)->buf,0x12121212);
	//printchar(&g,'a');

	return g;
	
	//printf("\n%u %u %u %u %u %u",console_width,buf->Width,font->width,console_height,buf->Height,font->height);
	//return g;
}
void move_cursor(graphics_context* g, UINT32 x, UINT32 y){
	g->cursor_x=x;
	g->cursor_y=y;
}

void get_cursor_pos(graphics_context* g, UINT32 *x, UINT32 *y){
	*x=g->cursor_x;
	*y=g->cursor_y;
}

void cursor_left(graphics_context* g){
	if (g->cursor_x==0){
		if(g->cursor_y==0){
			return;
		}
		else{
			g->cursor_y--;
		}
		g->cursor_x=g->console_width-1;
	}
	else{
		g->cursor_x--;
	}
}
void cursor_right(graphics_context* g){
	if (g->cursor_x>=g->console_width){
		g->cursor_x=0;
		g->cursor_y++;
	}
	else{
		g->cursor_x++;
	}
}
void cursor_up(graphics_context* g){
	if(g->cursor_y>0){
		g->cursor_y--;
	}
}
void cursor_down(graphics_context* g){
	if(g->cursor_y<g->console_height){
		g->cursor_y++;
	}
}

uint8_t cursor[32]={
	0b10000000,0b00000000,
	0b11000000,0b00000000,
	0b11100000,0b00000000,
	0b11110000,0b00000000,
	0b11111000,0b00000000,
	0b11111100,0b00000000,
	0b11111110,0b00000000,
	0b11111111,0b00000000,
	0b11111111,0b10000000,
	0b11111111,0b11000000,
	0b11111111,0b11100000,
	0b11111100,0b00000000,
	0b11111110,0b00000000,
	0b11101111,0b00000000,
	0b11000111,0b10000000,
	0b10000011,0b00000000
};
uint32_t cursor_buffer[16*16];


bool mouse_drawn=false;
volatile bool mouse_lock=false;
int mouse_cursor_x=0;
int mouse_cursor_y=0;
void move_mouse(int x, int y){
	//uint64_t flags=enter_critical();
	//clear_mouse();
	mouse_cursor_x=x;
	mouse_cursor_y=y;
	//draw_mouse();
	//exit_critical(flags);
}

uint8_t x_vis=0, y_vis=0;

void draw_mouse(Framebuffer* f){
	mouse_drawn=true;

	x_vis=MIN(16,f->Width-mouse_cursor_x);
	y_vis=MIN(16,f->Height-mouse_cursor_y);

	for (int y = 0; y < y_vis; ++y){
		for (int i = 0; i < MIN(x_vis,8); ++i){
			cursor_buffer[16*y+i]=ReadPixel_32bpp(f,mouse_cursor_x+i,mouse_cursor_y+y);
			if(cursor[y*2]>>(7-(i%8))&0x1){
				PlotPixel_32bpp(f,mouse_cursor_x+i,mouse_cursor_y+y,0xffffffff);
			}
		}
		for (int i = 0; i < x_vis-8; ++i){
			cursor_buffer[16*y+8+i]=ReadPixel_32bpp(f,mouse_cursor_x+8+i,mouse_cursor_y+y);
			if(cursor[y*2+1]>>(7-(i%8))&0x1){
				PlotPixel_32bpp(f,mouse_cursor_x+8+i,mouse_cursor_y+y,0xffffffff);
			}
		}
	}
}
void clear_mouse(Framebuffer* f){
	if (mouse_drawn){
		mouse_drawn=false;
		for (int y = 0; y < y_vis; ++y){
			for (int x = 0; x < x_vis; ++x){
				PlotPixel_32bpp(f,mouse_cursor_x+x,mouse_cursor_y+y,cursor_buffer[16*y+x]);
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
	memcpy(dest->BaseAddress,dest->BaseAddress,src->BufferSize);
}