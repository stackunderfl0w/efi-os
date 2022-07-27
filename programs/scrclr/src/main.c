//#include "graphics.h"
#include <stdbool.h>
unsigned char x=0;

void draw(char* buf){
	while(true){
		for (int i = 0; i < 100000; ++i)
		{
			buf[i]=x;
		}
	}
}
int _start(char* buf)
{
	draw(buf);
	//clrscr(buf, 0x29401930);
	
	return 0;
}