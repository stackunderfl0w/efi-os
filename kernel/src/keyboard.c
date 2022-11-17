#include "keyboard.h"
#include "graphics.h"
#include "ctype.h"
#include "stdio.h"
extern graphics_context* k_context;
int modifiers=0;
uint8_t previous_key=0;
const char ASCIITable[] = {
		 0 ,  0 , '1', '2',
		'3', '4', '5', '6',
		'7', '8', '9', '0',
		'-', '=',  0 ,  0 ,
		'q', 'w', 'e', 'r',
		't', 'y', 'u', 'i',
		'o', 'p', '[', ']',
		 0 ,  0 , 'a', 's',
		'd', 'f', 'g', 'h',
		'j', 'k', 'l', ';',
		'\'','`',  0 , '\\',
		'z', 'x', 'c', 'v',
		'b', 'n', 'm', ',',
		'.', '/',  0 , '*',
		 0 , ' '
	};
char Translate(uint8_t scancode, bool uppercase){
	if (scancode > 58) return 0;

	if (uppercase){
		return ASCIITable[scancode] - 32;
	}
	return ASCIITable[scancode];
}
bool callbacks_enabled=false;
void (*callback)(int, int)=NULL; 

void handle_key(uint8_t keycode){
	//kprintf("%u\n",(uint64_t)keycode);
	int final_keycode=0;
	if (keycode==0xE0 && previous_key==0){
		previous_key=keycode;
		return;
	}

	modifiers&=~(MODCODE_KEYUP|MODCODE_KEYDOWN);
	modifiers|=(keycode&0x80)?MODCODE_KEYUP:MODCODE_KEYDOWN;
	if (previous_key==0xE0){
		previous_key=0;
		switch (keycode){
			case Left_Arrow:
				final_keycode=KEYCODE_LEFT;
				break;
			case Right_Arrow:
				final_keycode=KEYCODE_RIGHT;
				break;
			case Up_Arrow:
				final_keycode=KEYCODE_UP;
				break;
			case Down_Arrow:
				final_keycode=KEYCODE_DOWN;
				break;
		}
	}
	else{
		switch (keycode){
			case LeftShift:
				final_keycode=KEYCODE_LSHIFT;
				modifiers|=MODCODE_LSHIFT;
				break;
			case LeftShift + 0x80:
				final_keycode=KEYCODE_LSHIFT;
				modifiers&= ~MODCODE_LSHIFT;
				break;
			case RightShift:
				final_keycode=KEYCODE_RSHIFT;
				modifiers|=MODCODE_RSHIFT;
				break;
			case RightShift + 0x80:
				final_keycode=KEYCODE_RSHIFT;
				modifiers&= ~MODCODE_RSHIFT;
				break;
			case Enter:
				final_keycode=KEYCODE_RETURN;
				break;
			case Spacebar:
				final_keycode=KEYCODE_SPACE;
				break;
			case BackSpace:
				final_keycode=KEYCODE_BACKSPACE;
				break;
			default:
				final_keycode=Translate(keycode&~0x80,false);
				break;
		}
	}

	if(callbacks_enabled){
		//kprintf("\nKEY%u MOD%u\n",final_keycode,modifiers);
		(*callback)(final_keycode,modifiers);
		return;
	}
	switch(final_keycode){
		case KEYCODE_LEFT:
			cursor_left(k_context,1);
			return;
		case KEYCODE_RIGHT:
			cursor_right(k_context,1);
			return;
		case KEYCODE_UP:
			cursor_up(k_context,1);
			return;
		case KEYCODE_DOWN:
			cursor_down(k_context,1);
			return;
		case KEYCODE_RETURN:
			printchar(k_context,'\n');
			return;
		case KEYCODE_SPACE:
			printchar(k_context,' ');
			return;
		case KEYCODE_BACKSPACE:
			deletechar(k_context);
			return;
	}

	char ascii = Translate(keycode, modifiers&MODCODE_SHIFT);
	if (isprint(ascii)){
		printchar(k_context,ascii);
	}
}

void set_keyboard_callback(void (*cb)(int,int)){
	callback=cb;
	callbacks_enabled=true;
}