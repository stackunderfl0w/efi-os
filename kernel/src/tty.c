#include "tty.h"
#include "typedef.h"
#include "graphics.h"
#include "memory.h"
#include "scheduler.h"
#include "keyboard.h"
#include "ctype.h"
//extern graphics_context* k_context;


void tty_write(tty* tt,char* f){
	char c;
	while((c=*f++)){
		if(c!=27){
			printchar(tt->g,c);
		}
		else{//escape sequence
			if(*f++=='['){//required next character of ascii escape sequence
				bool reading=true;
				int x=0,y=0;
				bool second=false;
				bool no_input=true;
				while (reading){
					reading=false;
					switch(c=*f++){
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
							move_cursor(tt->g,x,y);
							break;
						case 'A':
							cursor_up(tt->g,no_input?1:x);
							break;
						case 'B':
							cursor_down(tt->g,no_input?1:x);
							break;
						case 'C':
							cursor_right(tt->g,no_input?1:x);
							break;
						case 'D':
							cursor_left(tt->g,no_input?1:x);
							break;
						case 's':
							get_cursor_pos(tt->g,&tt->saved_x,&tt->saved_y);
							break;
						case 'u':
							move_cursor(tt->g,tt->saved_x,tt->saved_y);
							break;
						case 'm':
							int itt=second?2:1;//inlining this fails for some reason
							for (int i = 0; i < itt; ++i){
								uint32_t* target=(i?y:x)/10==3?&tt->g->foreground_color:&tt->g->background_color;
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

void kb_callback(int keycode, int modifiers){
	if(modifiers&MODCODE_KEYDOWN){
		if(isprint(keycode)||keycode=='\n'){
			//fputc(keycode,stdout);
			//fputc((char)keycode,stdin);
		}
	}
}

tty init_tty_0(graphics_context* g){
	//set_keyboard_callback(kb_callback);
	return (tty){g,0,0};
}
tty* init_tty(graphics_context* kg){
	return NULL;
}
