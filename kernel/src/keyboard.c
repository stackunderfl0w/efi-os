#include "keyboard.h"
bool isLeftShiftPressed;
bool isRightShiftPressed;
uint8_t previous=0;
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

void handle_key(unsigned char keycode){
	//print(to_hstring(keycode));
	if (keycode==0xE0 && previous==0){
		previous=keycode;
		return;
	}
	if (previous==0xE0){
		switch (keycode){
			case Left_Arrow:
				cursor_left();
				return;
			case Right_Arrow:
				cursor_right();
				return;
			case Up_Arrow:
				cursor_up();
				return;
			case Down_Arrow:
				cursor_down();
				return;
		}
	}
	switch (keycode){
		case LeftShift:
			isLeftShiftPressed = true;
			return;
		case LeftShift + 0x80:
			isLeftShiftPressed = false;
			return;
		case RightShift:
			isRightShiftPressed = true;
			return;
		case RightShift + 0x80:
			isRightShiftPressed = false;
			return;
		case Enter:
			printchar('\n');
			return;
		case Spacebar:
			printchar(' ');
			return;
		case BackSpace:
			//deletechar();
			scroll_console();
			return;
	}

	char ascii = Translate(keycode, isLeftShiftPressed | isRightShiftPressed);

	if (ascii != 0){
		printchar(ascii);
	}	
}