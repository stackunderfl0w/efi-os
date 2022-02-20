#include "bitmap-font.h"
#include "serial.h"
//https://www.win.tue.nl/~aeb/linux/kbd/font-formats-1.html

//create a bitmap font from a loaded file
bitmap_font load_font(CHAR8* buffer){
	bitmap_font font;
	if (((CHAR16*)buffer)[0]==PSF1_MAGIC){
		print_serial("PSF1 font found");
		psf1_header* header=(psf1_header*)buffer;

		font.numglyph=256*(1+(header->mode&PSF1_MODE512));
		font.bytesperglyph=header->charsize;

		font.height=header->charsize;
		font.width=8;
		font.buffer=buffer+4;
	}
	else if(((uint32_t*)buffer)[0]==PSF2_MAGIC){
		print_serial("PSF2 font found");
		psf2_header* header=(psf2_header*)buffer;
		if(header->flags&PSF2_HAS_UNICODE_TABLE){
			//Print(L"Unicode table found, todo");
			//This wont be necesary until/if I implement unicode text, seems i can just ignore the tables entirely for now.
		}

		font.numglyph=header->numglyph;
		font.bytesperglyph=header->bytesperglyph;

		font.height=header->height;
		font.width=header->width;
		font.buffer=buffer+32;
		//Print(L"%u",font.width);

	}
	else{
		print_serial("Invalid font found");
	}
	return font;

}