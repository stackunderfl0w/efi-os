#include "tty.h"
#include "typedef.h"
#include "graphics.h"
#include "memory.h"
#include "scheduler.h"
#include "keyboard.h"
#include "ctype.h"
#include "filesystem.h"
#include "stdio.h"
#include "paging.h"
extern graphics_context* global_context;


void tty_write(tty* tt,char* f){
	char c;
	while((c=*f++)){
		if(c!=27&&!tt->reading_escape){
			printchar(tt->g,c);
			continue;
		}
		else if(c==27){//start of escape sequence
			tt->reading_escape=true;
			tt->esc_x=tt->esc_y=0;
			tt->esc_second=false;
			tt->esc_no_input=true;
			tt->maybe_escape=true;
			continue;
		}
		else if(tt->maybe_escape){
			if(c=='['){//required next character of ascii escape sequence
				tt->maybe_escape=false;
				continue;
			}
		}
		else{//inside escape sequence
			tt->reading_escape=false;
			switch(c){
				case '0'...'9':
					if(!tt->esc_second){
						tt->esc_x=tt->esc_x*10+(c-'0');
					}
					else{
						tt->esc_y=tt->esc_y*10+(c-'0');
					}
					tt->esc_no_input=false;
					tt->reading_escape=true;
					break;
				case ';':
					tt->reading_escape=true;
					tt->esc_second=true;
					break;
				case 'H':case'f':
					move_cursor(tt->g,tt->esc_x,tt->esc_y);
					break;
				case 'A':
					cursor_up(tt->g,tt->esc_no_input?1:tt->esc_x);
					break;
				case 'B':
					cursor_down(tt->g,tt->esc_no_input?1:tt->esc_x);
					break;
				case 'C':
					cursor_right(tt->g,tt->esc_no_input?1:tt->esc_x);
					break;
				case 'D':
					cursor_left(tt->g,tt->esc_no_input?1:tt->esc_x);
					break;
				case 's':
					get_cursor_pos(tt->g,&tt->saved_x,&tt->saved_y);
					break;
				case 'u':
					move_cursor(tt->g,tt->saved_x,tt->saved_y);
					break;
				case 'm':{
					int itt=tt->esc_second?2:1;//inlining this fails for some reason
					for (int i = 0; i < itt; ++i){
						uint32_t* target=(i?tt->esc_y:tt->esc_x)/10==3?&tt->g->foreground_color:&tt->g->background_color;
						int col=(i?tt->esc_y:tt->esc_x)%10;
						*target=0xff000000+(col&0b1)*0x00ff0000+(col>>1&0b1)*0x0000ff00+(col>>2&0b1)*0x000000ff;
					}
				}
					break;
				default:
					break;
			}
		}
	}
}


void kb_callback(int keycode, int modifiers){
	if(modifiers&MODCODE_KEYDOWN){
		if(isprint(keycode)||keycode=='\n'){
			//fputc(keycode,stdout);
			//fputc((char)keycode,stdin);
		}
	}
}

tty init_tty_0(graphics_context* g){
	return (tty){.g=g,.saved_x=0,.saved_y=0,.reading_escape=false};
}
tty* init_tty(int id, bitmap_font* font){
	//set_keyboard_callback(kb_callback);
	//create stdio files
	uint64_t flags=enter_critical();
	char fn[256];
	ksprintf(fn,"/dev/tty%d",id);
	mkdir(fn,0);
	chdir(fn);
	mkfifo("tty_in",0);
	mkfifo("tty_out",0);
	chdir("../..");
	//create new fb

	Framebuffer* fb=alloc_framebuffer(global_context->buf->Width,global_context->buf->Height,(void*)0x6000000000+0x100000000*id);
	//create a new graphics context with that framebuffer

	graphics_context *g=kmalloc(sizeof(graphics_context)); 
	*g=init_text_overlay(fb, font);
	tty* ty=kmalloc(sizeof(tty));
	*ty=(tty){.g=g,.saved_x=0,.saved_y=0,.reading_escape=false};
	exit_critical(flags);


	return ty;
}
