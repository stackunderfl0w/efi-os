#include <stdarg.h>
#include "graphics.h"
#include "stdlib.h"
char printf_buf[512];
char* temp;
void vsprintf(char* str, const char* format, va_list ap){
	//memset(printf_buf,0,512);
	int output_index=0;
	int ti=0;

	for (int i = 0; format[i]; ++i){
		if(format[i]=='%'){
			switch(format[++i]){
				case 'x':
					temp=to_hstring(va_arg(ap,uint64_t));
					ti=0;
					while(temp[ti]){
						str[output_index++]=temp[ti++];
					}	
					break;				
				case 'p':
					temp=to_hstring(va_arg(ap,uint64_t));
					ti=0;
					while(temp[ti]){
						str[output_index++]=temp[ti++];
					}	
					break;
				case 'u':
					/*temp=to_string(va_arg(ap,uint64_t));
					ti=0;
					while(temp[ti]){
						str[output_index++]=temp[ti++];
					}*/
					itoa(va_arg(ap,uint64_t),str+output_index,10);
					while(str[output_index]){
						output_index++;
					}
					break;
				case 'f':
					temp=to_string_double(va_arg(ap,double),6);
					ti=0;
					while(temp[ti]){
						str[output_index++]=temp[ti++];
					}	
					break;
				case 's':
					temp=va_arg(ap,char*);
					ti=0;
					while(temp[ti]){
						str[output_index++]=temp[ti++];
					}	
					break;
				case 'c':
					str[output_index++]=(char)va_arg(ap,int);
					break;

			}
		}
		else{
			str[output_index++]=format[i];
		}
	}
	str[output_index]=0;
	va_end(ap);
}
void sprintf(char* str, const char* format, ...){
	va_list arg;
	va_start (arg, format);
	sprintf (str, format, arg);
	va_end (arg);
}
void printf(const char* format, ... ){
	va_list args;
	va_start (args, format);
	vsprintf(printf_buf, format, args);
	va_end (args);
	print(printf_buf);
}