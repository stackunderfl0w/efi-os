#include "tty.h"
#include "typedef.h"
#include "file_struct.h"
#include "graphics.h"
#include "memory.h"
FILE* stdout;
graphics_context* g;
int saved_x=0,saved_y=0;


void sync(FILE* f){
    while(f->write_head-f->read_head){
        char c=read(f);
        if(c!=27){
            printchar(g,c);
        }
        else{//escape sequence
            if(read(f)=='['){//required next character of ascii escape sequence
                bool reading=true;
                int x=0,y=0;
                bool second=false;
                while (reading){
                    reading=false;
                    switch(c=read(f)){
                        case '0'...'9':
                            if(!second){
                                x=x*10+(c-'0');
                            }
                            else{
                                y=y*10+(c-'0');
                            }
                            reading=true;
                            break;
                        case ';':
                            reading=true;
                            second=true;
                            break;
                        case 'H':case'f':
                            move_cursor(g,x,y);
                            break;
                        case 'A':
                            break;
                        case 's':
                            get_cursor_pos(g,&saved_x,&saved_y);
                            break;
                        case 'u':
                            move_cursor(g,saved_x,saved_y);
                            break;
                        default:
                            break;

                    }
                }
            }
        }
    }
}
void write(FILE* f, char c){
    *f->write_head++=c;
    if(f->write_head==f->end||c=='\n'){
        sync(f);
    }
}
char read(FILE* f){
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

tty init_tty_0(graphics_context* kg, FILE* stdout_0, char* buf, unsigned buf_size){
    stdout=stdout_0;
    stdout->base=buf;
    stdout->end=stdout->base+buf_size;
    stdout->read_head=stdout->base;
    stdout->write_head=stdout->base;

    stdout->write=write;
    stdout->read=read;
    stdout->sync=sync;

    g=kg;
}
tty init_tty(graphics_context* kg){
	stdout=calloc(sizeof(FILE));
	stdout->base=calloc(8192);
	stdout->end=stdout->base+8192;
	stdout->read_head=stdout->base;
	stdout->write_head=stdout->base;

	stdout->write=write;
    stdout->read=read;
    stdout->sync=sync;

    g=kg;

}

void tty_loop(tty this){

}