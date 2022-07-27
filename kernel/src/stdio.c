#include <stdarg.h>
#include "graphics.h"
#include "stdlib.h"
#include "stdio.h"
#include "loop.h"

void aquire_lock(_Atomic volatile bool *lock){
	while(*lock){
		busyloop(1);
	}
	*lock=true;
}
void release_lock(_Atomic volatile bool *lock){
	*lock=false;
}
//currently works with base 8, 10, and 16
const char hex_ascii[16]="0123456789abcdef";
char* format_number(char* buf, size_t n, size_t base){
    //longest posible should be 21 characters. rounded up to 32
    char tmp[32];
    int i=0;
    //fill temp buffer in reverse
    if(base==16||base==8){
        unsigned mask=base-1;
        int shift=base==16?4:3;
        do{
            tmp[i++]=hex_ascii[n&mask];
        }while(n>>=shift);

    }
        //treat unknown as base 10
    else{
        do{
            tmp[i++]=(char)('0'+n%10);
            n/=10;
        }while(n);
    }
    //copy over to destination buffer
    while(i){
        *buf++=tmp[--i];
    }
    //return end of string
    return buf;
}
char* format_double(char* buf, double x, uint64_t precision){
	buf=format_number(buf,(uint64_t)x,10);
	double remainder=x-(uint64_t)x;
	*buf++='.';
	int places=0;
	//i need to find a faster way to do this
	while(places++<precision){
		*buf++='0'+(uint64_t)(remainder*=10)%10;
	}
	return buf;
}
//no need to specify between 32 and 64 bit. It gets cast to 64 bit anyways
void vsprintf(char* str, const char* format, va_list ap){
	char* temp;
	for (const char* inx = format; *inx; ++inx){
		if(*inx=='%'){
			switch(*++inx){
				case 'p':
					*str++='0';
					*str++='x';
				case 'x':
					str=format_number(str,va_arg(ap,uint64_t),16);
					break;				
					//i dont care enough about negative numbers for now
				case 'u':case 'd':case 'i':
					str=format_number(str,va_arg(ap,uint64_t),10);
					break;
				case 'f':
					str=format_double(str,va_arg(ap,double),6);
					break;
				case 's':
					temp=va_arg(ap,char*);
					while(*temp){
						*str++=*temp++;
					}	
					break;
				case 'c':
					*str++=(char)va_arg(ap,int);
					break;

			}
		}
		else{
			*str++=*inx;
		}
	}
	*str=0;
	va_end(ap);
}
void sprintf(char* str, const char* format, ...){
	va_list arg;
	va_start (arg, format);
	sprintf (str, format, arg);
	va_end (arg);
}
void old_printf(const char* format, ... ){
	char printf_buf[512];
	va_list args;
	va_start (args, format);
	vsprintf(printf_buf, format, args);
	va_end (args);
	//aquire_lock();
	//print(printf_buf);
	//release_lock();
}
void printf(const char* format, ... ){
	char printf_buf[512];
	va_list args;
	va_start (args, format);
	vsprintf(printf_buf, format, args);
	va_end (args);
	aquire_lock(&stdout->io_lock);
	//print(printf_buf);
	/* (char* s=printf_buf; *s; ++s){
		//*stdout->write_head++=*s;
		fputc(*s,stdout);
	}*/
	fputs(printf_buf,stdout);
	release_lock(&stdout->io_lock);
}

int fgetc(FILE* f){
    return f->read(f);
}
int fputc(int c, FILE* f){
	f->write(f,(char)c);
	return c;
}

int fputs(const char *str, FILE* f){
	for (const char* s=str; *s; ++s){
		//*stdout->write_head++=*s;
		f->write(f,*s);
	}
	return 0;
}
