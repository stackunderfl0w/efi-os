#include "graphics.h"

Framebuffer* globalBuf;
bitmap_font* console_font;

void PlotPixel_32bpp(int x, int y, uint32_t pixel){
	*((uint32_t*)((char*)globalBuf->BaseAddress + 4 * globalBuf->PixelsPerScanLine * y + 4 * x)) = pixel;
}
uint32_t ReadPixel_32bpp(int x, int y){
	return *(uint32_t*)(globalBuf->BaseAddress + 4 * globalBuf->PixelsPerScanLine * y + 4 * x);
}


UINT32 cursor_x,cursor_y;
UINT32 console_width,console_height;
bitmap_font* console_font;
char string_buf[256]={0};

char* to_string(size_t x){
	if(x==0){
		string_buf[0]='0';
		string_buf[1]=0;
		return string_buf;
	}
	size_t index=0;
	size_t size=0;
	size_t temp=x;
	while(temp/=10){
		size++;
	}
	while(x){
		string_buf[size-index]='0'+x%10;
		x/=10;
		index++;
		//print("x");
	}
	string_buf[size+1]=0x00;
	return string_buf;
}

char* to_hstring(size_t x){
	if(x==0){
		string_buf[0]='0';
		string_buf[1]='x';
		string_buf[2]='0';
		string_buf[3]=0;
		return string_buf;
	}
	uint64_t index=0;
	uint64_t size=0;
	uint64_t temp =x;

	while(temp/=16){
		size++;
	}


	string_buf[0]='0';
	string_buf[1]='x';

	while(x){
		string_buf[size-index+2]= x%16<10? '0'+x%16: '7'+x%16;
		x/=16;
		index++;
		//print("x");
	}
	string_buf[size+3]=0x00;
	return string_buf;
}
char* to_hstring_noformat(size_t x){
	if(x==0){
		string_buf[0]='0';
		string_buf[1]=0;
		return string_buf;
	}
	uint64_t index=0;
	uint64_t size=0;
	uint64_t temp =x;

	while(temp/=16){
		size++;
	}


	string_buf[0]='0';
	string_buf[1]='x';

	while(x){
		string_buf[size-index]= x%16<10? '0'+x%16: '7'+x%16;
		x/=16;
		index++;
		//print("x");
	}
	string_buf[size+1]=0x00;
	return string_buf;
}
char* to_string_double(double x, uint64_t precision){
	to_string((uint64_t)x);
	int index=0;
	while(string_buf[++index]);
	double remainder=x-(uint64_t)x;
	string_buf[index++]='.';

	int places=0;
	while(places++<precision){
		string_buf[index++]='0'+(uint64_t)(remainder*=10)%10;
	}
	string_buf[index]=0;

	return string_buf;
}
long atoi(const char* buf){
	unsigned long long output=0;
	int index=0;
	int sign =1;
	if(buf[index]=='-'){
		sign=-1;
		index++;
	}
	while(buf[index]>='0'&&buf[index]<='9'&&index<20){
		printf("index %c \n",buf[index]);

		output*=10;
		output+=buf[index++]-'0';
	}
	return output*sign;
}
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
	if(cursor_y>=console_height){
		cursor_y--;
		scroll_console();       
	}
	if (chr=='\n'){
		cursor_x=0;
		cursor_y++;
		return;
	}
	putchar(cursor_x*console_font->width, cursor_y*console_font->height, chr);
	cursor_x++;
	if (cursor_x>=console_width){
		cursor_x=0;
		cursor_y++;
	}
}
void print(const char* str){
	//print_serial(str);
	for (int i = 0; str[i]!=0; ++i)
	{
		printchar(str[i]);
	}
}
char printf_buf[512];
char* temp;
void printf(const char* str, ...){
	memset(printf_buf,0,512);
	int output_index=0;
	int ti=0;

	va_list ap;
	va_start(ap,str);

	for (int i = 0; str[i]; ++i){
		if(str[i]=='%'){
			switch(str[++i]){
				case 'x':
					temp=to_hstring(va_arg(ap,uint64_t));
					ti=0;
					while(temp[ti]){
						printf_buf[output_index++]=temp[ti++];
					}	
					break;				
				case 'p':
					temp=to_hstring(va_arg(ap,uint64_t));
					ti=0;
					while(temp[ti]){
						printf_buf[output_index++]=temp[ti++];
					}	
					break;
				case 'u':
					temp=to_string(va_arg(ap,uint64_t));
					ti=0;
					while(temp[ti]){
						printf_buf[output_index++]=temp[ti++];
					}	
					break;
				case 'f':
					temp=to_string_double(va_arg(ap,double),6);
					ti=0;
					while(temp[ti]){
						printf_buf[output_index++]=temp[ti++];
					}	
					break;
				case 's':
					temp=va_arg(ap,char*);
					ti=0;
					while(temp[ti]){
						printf_buf[output_index++]=temp[ti++];
					}	
					break;
				case 'c':
					printf_buf[output_index++]=(char)va_arg(ap,int);
					break;

			}
		}
		else{
			printf_buf[output_index++]=str[i];
		}
	}
	va_end(ap);
	print(printf_buf);
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
	asm volatile("sti");
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
	clear_mouse();
	mouse_cursor_x=x;
	mouse_cursor_y=y;
	draw_mouse();
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
	uint64_t flags;
	asm volatile("# __raw_save_flags\n\t"
		"pushf ; pop %0"
		: "=rm" (flags)
		: /* no input */
		: "memory");
	if (flags&0x200){
		asm volatile("cli");
	}
	clear_mouse();
	memcpy(globalBuf->BaseAddress,globalBuf->BaseAddress+(globalBuf->PixelsPerScanLine*console_font->height)*4,globalBuf->PixelsPerScanLine*console_font->height*(console_height-1)*4);
	memset(globalBuf->BaseAddress+globalBuf->PixelsPerScanLine*console_font->height*(console_height-1)*4,
		0,
		globalBuf->BufferSize-(4*globalBuf->PixelsPerScanLine*console_font->height*(console_height-1)));
	draw_mouse();
	asm volatile("sti");
}


