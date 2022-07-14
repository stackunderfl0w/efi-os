#include "tty.h"
#include "typedef.h"
#include "file_struct.h"
#include "graphics.h"
#include "memory.h"
FILE* stdout;
void sync(FILE* f){
    while(f->write_head-f->read_head){
        printchar(*f->read_head);
        *f->read_head++=0;
    }
    f->read_head=f->write_head=f->base;
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
        f->read_head=f->base;
        f->write_head=f->base;
    }
    return tmp;
}


tty init_tty(){
	stdout=calloc(sizeof(FILE));
	stdout->base=calloc(8192);
	stdout->end=stdout->base+8192;
	stdout->read_head=stdout->base;
	stdout->write_head=stdout->base;

	stdout->write=write;
    stdout->read=read;
    stdout->sync=sync;

}

void tty_loop(tty this){

}