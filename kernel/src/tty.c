#include "tty.h"
#include "typedef.h"
#include "file_struct.h"
#include "graphics.h"
#include "memory.h"
#include "scheduler.h"
#include "keyboard.h"
#include "ctype.h"
FILE* stdout;
FILE* stdin;
graphics_context* g;
int saved_x=0,saved_y=0;
extern graphics_context* k_context;


void tty_sync(FILE* f){
	while(f->write_head-f->read_head){
		char c=tty_read(f);
		if(c!=27){
			printchar(k_context,c);
		}
		else{//escape sequence
			if(tty_read(f)=='['){//required next character of ascii escape sequence
				bool reading=true;
				int x=0,y=0;
				bool second=false;
				bool no_input=true;
				while (reading){
					reading=false;
					switch(c=tty_read(f)){
						case '0'...'9':
							if(!second){
								x=x*10+(c-'0');
							}
							else{
								y=y*10+(c-'0');
							}
							no_input=false;
							reading=true;
							break;
						case ';':
							reading=true;
							second=true;
							break;
						case 'H':case'f':
							move_cursor(k_context,x,y);
							break;
						case 'A':
							cursor_up(k_context,no_input?1:x);
							break;
						case 'B':
							cursor_down(k_context,no_input?1:x);
							break;
						case 'C':
							cursor_right(k_context,no_input?1:x);
							break;
						case 'D':
							cursor_left(k_context,no_input?1:x);
							break;
						case 's':
							get_cursor_pos(k_context,&saved_x,&saved_y);
							break;
						case 'u':
							move_cursor(k_context,saved_x,saved_y);
							break;
						case 'm':
							int itt=second?2:1;//inlining this fails for some reason
							for (int i = 0; i < itt; ++i){
								uint32_t* target=(i?y:x)/10==3?&k_context->foreground_color:&k_context->background_color;
								int col=(i?y:x)%10;
								*target=0xff000000+(col&0b1)*0x00ff0000+(col>>1&0b1)*0x0000ff00+(col>>2&0b1)*0x000000ff;
							}
							break;
						default:
							break;
					}
				}
			}
		}
	}
}
void tty_write(FILE* f, char c){
	if(f->write_head==f->end||c=='\n'){
		if(f->flags&IO_NO_SYNC){
			release_lock(&f->io_lock);
			while(f->write_head==f->end){
				yield();
			}
			aquire_lock(&f->io_lock);
		}
		else
			tty_sync(f);
	}
	*f->write_head++=c;
}
char tty_read(FILE* f){
	if (f->read_head==f->write_head){
		return 0;
	}
	char tmp=*f->read_head;
	*f->read_head++=0;
	if(f->read_head==f->write_head){
		f->read_head=f->write_head=f->base;
	}
	return tmp;
}
void kb_callback(int keycode, int modifiers){
	if(modifiers&MODCODE_KEYDOWN){
		if(isalpha(keycode)||keycode=='\n'){
			fputc(keycode,stdout);
			fputc((char)keycode,stdin);
		}
	}
}
void stdin_sync(FILE* f){

}

tty init_tty_0(FILE* stdout_0, FILE* stdin_0, char* stdout_buf, char* stdin_buf, unsigned buf_size){
	stdout=stdout_0;
	stdout->base=stdout_buf;
	stdout->end=stdout->base+buf_size;
	stdout->read_head=stdout->base;
	stdout->write_head=stdout->base;

	stdout->write=tty_write;
	stdout->read=tty_read;
	stdout->sync=tty_sync;

	stdout->io_lock=false;

	stdout->flags=IO_UNBUFFERED;

	stdin=stdin_0;
	stdin->base=stdin_buf;
	stdin->end=stdin->base+buf_size;
	stdin->read_head=stdin->base;
	stdin->write_head=stdin->base;

	stdin->write=tty_write;
	stdin->read=tty_read;
	stdin->sync=stdin_sync;

	stdin->io_lock=false;

	stdin->flags=IO_UNBUFFERED|IO_NO_SYNC;
	set_keyboard_callback(kb_callback);
}
tty init_tty(graphics_context* kg){
	stdout=calloc(sizeof(FILE));
	stdout->base=calloc(8192);
	stdout->end=stdout->base+8192;
	stdout->read_head=stdout->base;
	stdout->write_head=stdout->base;

	stdout->write=tty_write;
	stdout->read=tty_read;
	stdout->sync=tty_sync;

	g=kg;
}

void tty_loop(tty this){

}