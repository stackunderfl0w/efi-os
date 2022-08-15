//#include "graphics.h"
#include <stdbool.h>
unsigned char x=0;

void draw(char* buf){
	while(true){
		for (int i = 0; i < 1000000; ++i)
		{
			buf[i]=x;
		}
		x+=257;
	}
}
int _start(char* buf)
{
	draw(buf);
	//clrscr(buf, 0x29401930);
	
	return 0;
}