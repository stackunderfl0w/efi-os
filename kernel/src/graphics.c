#include "graphics.h"
#include "stdlib.h"

Framebuffer* globalBuf;
Framebuffer back_buffer;
bool back_buffer_enabled;
bitmap_font* console_font;

inline void PlotPixel_32bpp(int x, int y, uint32_t pixel){
	*((uint32_t*)((char*)globalBuf->BaseAddress + 4 * globalBuf->PixelsPerScanLine * y + 4 * x)) = pixel;
}
inline uint32_t ReadPixel_32bpp(int x, int y){
	return *(uint32_t*)(globalBuf->BaseAddress + 4 * globalBuf->PixelsPerScanLine * y + 4 * x);
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


UINT32 cursor_x,cursor_y;
UINT32 console_width,console_height;
bitmap_font* console_font;


void putchar(UINT32 x, UINT32 y, CHAR8 chr){
	clear_mouse();
	uint32_t x_vis=MIN(console_font->width,globalBuf->Width-x);
	uint32_t y_vis=MIN(console_font->height,globalBuf->Height-y);

	for (uint32_t y2 = 0; y2 < y_vis; ++y2){
		for (uint32_t x2 = 0; x2 < x_vis; ++x2)
		{
			if ((console_font->buffer[console_font->bytesperglyph*chr+(y2*((console_font->width+7)/8))+x2/8]>>(7-(x2%8)))&0x1){
				PlotPixel_32bpp(x+x2,y+y2,0xffffffff);
			}
		}
	}
	draw_mouse();
}

void clearchar(UINT32 x, UINT32 y){
	for (uint32_t y2 = 0; y2 < console_font->height; ++y2){
		for (uint32_t x2 = 0; x2 < console_font->width; ++x2)
		{
			PlotPixel_32bpp(x+x2,y+y2,0x00000000);
		}
	}
}

void deletechar(){
	cursor_left();
	clearchar(cursor_x*console_font->width, cursor_y*console_font->height);
}

void printchar(char chr){
	if (chr=='\n'){
		cursor_x=0;
		cursor_y++;
		return;
	}
	if(cursor_y>=console_height){
		cursor_y--;
		scroll_console();       
	}
	//save and calculate new position before printing char for multithreading
	int print_x=cursor_x,print_y=cursor_y;
	cursor_x++;
	if (cursor_x>=console_width){
		cursor_x=0;
		cursor_y++;
	}
	putchar(print_x*console_font->width, print_y*console_font->height, chr);
}
void print(const char* str){
	//print_serial(str);
	for (int i = 0; str[i]!=0; ++i)
	{
		printchar(str[i]);
	}
}




void clrscr(uint32_t color){
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
	uint64_t end=globalBuf->BufferSize/4;
	for (int i = 0; i < end; ++i){
		((uint32_t*)globalBuf->BaseAddress)[i]=color;
	}
	if (flags&0x200){
		asm volatile("sti");
	}
}

void init_text_overlay(Framebuffer* buf, bitmap_font* font){
	globalBuf=buf;
	console_font=font;

	console_width=(buf->Width/font->width);
	console_height=(buf->Height/font->height);
	cursor_x=0;
	cursor_y=0;
	
	clrscr(0x00000000);
	//printf("\n%u %u %u %u %u %u",console_width,buf->Width,font->width,console_height,buf->Height,font->height);

}
void move_cursor(UINT32 x, UINT32 y){
	cursor_x=x;
	cursor_y=y;
}

void get_cursor_pos(UINT32 *x, UINT32 *y){
	*x=cursor_x;
	*y=cursor_y;
}

void cursor_left(){
	if (cursor_x==0){
		if(cursor_y==0){
			return;
		}
		else{
			cursor_y--;
		}
		cursor_x=console_width-1;
	}
	else{
		cursor_x--;
	}
}
void cursor_right(){
	if (cursor_x>=console_width){
		cursor_x=0;
		cursor_y++;
	}
	else{
		cursor_x++;
	}
}
void cursor_up(){
	if(cursor_y>0){
		cursor_y--;
	}
}
void cursor_down(){
	if(cursor_y<console_height){
		cursor_y++;
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
	uint64_t flags=enter_critical();
	clear_mouse();
	mouse_cursor_x=x;
	mouse_cursor_y=y;
	draw_mouse();
	exit_critical(flags);
}

uint8_t x_vis=0, y_vis=0;

void draw_mouse(){
	mouse_drawn=true;

	x_vis=MIN(16,globalBuf->Width-mouse_cursor_x);
	y_vis=MIN(16,globalBuf->Height-mouse_cursor_y);

	for (int y = 0; y < y_vis; ++y){
		for (int i = 0; i < MIN(x_vis,8); ++i){
			cursor_buffer[16*y+i]=ReadPixel_32bpp(mouse_cursor_x+i,mouse_cursor_y+y);
			if(cursor[y*2]>>(7-(i%8))&0x1){
				PlotPixel_32bpp(mouse_cursor_x+i,mouse_cursor_y+y,0xffffffff);
			}
		}
		for (int i = 0; i < x_vis-8; ++i){
			cursor_buffer[16*y+8+i]=ReadPixel_32bpp(mouse_cursor_x+8+i,mouse_cursor_y+y);
			if(cursor[y*2+1]>>(7-(i%8))&0x1){
				PlotPixel_32bpp(mouse_cursor_x+8+i,mouse_cursor_y+y,0xffffffff);
			}
		}
	}
}
void clear_mouse(){
	if (mouse_drawn){
		mouse_drawn=false;
		for (int y = 0; y < y_vis; ++y){
			for (int x = 0; x < x_vis; ++x){
				PlotPixel_32bpp(mouse_cursor_x+x,mouse_cursor_y+y,cursor_buffer[16*y+x]);
			}
		}
	}
}
void get_display_resolution(UINT32 *x, UINT32 *y){
	*x=globalBuf->Width;
	*y=globalBuf->Height;
}

void scroll_console(){
	uint64_t flags=enter_critical();
	clear_mouse();
	int row_size_px=globalBuf->PixelsPerScanLine*console_font->height;
	//move everything up 1 line
	memcpy(globalBuf->BaseAddress, globalBuf->BaseAddress+(row_size_px)*4, row_size_px*(console_height-1)*4);
	//clear bottom line
	memset(globalBuf->BaseAddress+row_size_px*(console_height-1)*4, 0, globalBuf->BufferSize-(4*row_size_px*(console_height-1)));
	draw_mouse();
	exit_critical(flags);
}