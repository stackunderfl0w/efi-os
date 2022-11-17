#include <stdarg.h>
#include "graphics.h"
#include "stdlib.h"
#include "stdio.h"
#include "loop.h"

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
int kvsprintf(char* str, const char* format, va_list ap){
	char* temp;
	char* buf=str;

	for (const char* inx = format; *inx; ++inx){
		if(*inx=='%'){
			switch(*++inx){
				case 'p':
					*buf++='0';
					*buf++='x';
				case 'x':
					buf=format_number(buf,va_arg(ap,uint64_t),16);
					break;				
					//i dont care enough about negative numbers for now
				case 'u':case 'd':case 'i':
					buf=format_number(buf,va_arg(ap,uint64_t),10);
					break;
				case 'f':
					buf=format_double(buf,va_arg(ap,double),6);
					break;
				case 's':
					temp=va_arg(ap,char*);
					while(*temp){
						*buf++=*temp++;
					}	
					break;
				case 'c':
					*buf++=(char)va_arg(ap,int);
					break;

			}
		}
		else{
			*buf++=*inx;
		}
	}
	*buf=0;
	va_end(ap);
	return buf-str;
}
int ksprintf(char* str, const char* format, ...){
	va_list arg;
	va_start (arg, format);
	int i = kvsprintf (str, format, arg);
	va_end (arg);
	return i;
}
int kprintf(const char* format, ... ){
	char kprintf_buf[512];
	va_list args;
	va_start (args, format);
	int i = kvsprintf(kprintf_buf, format, args);
	va_end (args);
	fputs(kprintf_buf,stdout);
	return i;
}

int fgetc(FILE* f){
	aquire_lock(&f->io_lock);
	int c=f->read(f);
	release_lock(&f->io_lock);
	return c;

}
int fputc(int c, FILE* f){
	aquire_lock(&f->io_lock);
	f->write(f,(char)c);
	if(f->flags&IO_UNBUFFERED||(f->flags&IO_LINE_BUFFERED&&c=='\n'))
		f->sync(f);
	release_lock(&f->io_lock);
	return c;
}

int fputs(const char *str, FILE* f){
	aquire_lock(&f->io_lock);
	for (const char* s=str; *s; ++s){
		f->write(f,*s);
		if(f->flags&IO_LINE_BUFFERED&&*s=='\n')
			f->sync(f);
	}
	if(f->flags&IO_UNBUFFERED)
		f->sync(f);
	release_lock(&f->io_lock);
	return 0;
}

int fflush(FILE* f){
	f->sync(f);
}
