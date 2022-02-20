#pragma once
#include "typedef.h"
#include "graphics.h"



#define LeftShift	0x2A
#define RightShift	0x36
#define Enter		0x1C
#define BackSpace	0x0E
#define Spacebar	0x39
#define Up_Arrow	0x48
#define Left_Arrow	0x4B
#define Right_Arrow	0x4D
#define Down_Arrow	0x50

//matches ascii charset when possible.
enum actions{
	PRESS,
	UNPRESS,
};
enum keycodes{
	KEYCODE_UNKNOWN=0x00,
	KEYCODE_BACKSPACE=0x08,
	KEYCODE_TAB=0x09,
	KEYCODE_RETURN=0x0d,

	KEYCODE_SPACE=0x20,

	KEYCODE_EXCLAIM=0x21,
	KEYCODE_QUOTEDBL=0x22,
	KEYCODE_HASH=0x23,
	KEYCODE_DOLLAR=0x24,
	KEYCODE_PERCENT=0x25,
	KEYCODE_AMPERSAND=0x26,
	KEYCODE_QUOTE=0x27,
	KEYCODE_LEFTPAREN=0x28,
	KEYCODE_RIGHTPAREN=0x29,
	KEYCODE_ASTERISK=0x2a,
	KEYCODE_PLUS=0x2b,
	KEYCODE_COMMA=0x2c,
	KEYCODE_MINUS=0x2d,
	KEYCODE_PERIOD=0x2e,
	KEYCODE_SLASH=0x2f,

	KEYCODE_0=0x30,
	KEYCODE_1=0x31,
	KEYCODE_2=0x32,
	KEYCODE_3=0x33,
	KEYCODE_4=0x34,
	KEYCODE_5=0x35,
	KEYCODE_6=0x36,
	KEYCODE_7=0x37,
	KEYCODE_8=0x38,
	KEYCODE_9=0x39,

	KEYCODE_COLON=0x3a,
	KEYCODE_SEMICOLON=0x3b,
	KEYCODE_LESS=0x3c,
	KEYCODE_EQUALS=0x3d,
	KEYCODE_GREATER=0x3e,
	KEYCODE_QUESTION=0x3f,
	KEYCODE_AT=0x40,

	KEYCODE_LEFTBRACKET=0x5b,
	KEYCODE_BACKSLASH=0x5c,
	KEYCODE_RIGHTBRACKET=0x5d,
	KEYCODE_CARET=0x5e,
	KEYCODE_UNDERSCORE=0x5f,

	KEYCODE_BACKQUOTE=0x60,

	KEYCODE_a=0x61,
	KEYCODE_b=0x62,
	KEYCODE_c=0x63,
	KEYCODE_d=0x64,
	KEYCODE_e=0x65,
	KEYCODE_f=0x66,
	KEYCODE_g=0x67,
	KEYCODE_h=0x68,
	KEYCODE_i=0x69,
	KEYCODE_j=0x6a,
	KEYCODE_k=0x6b,
	KEYCODE_l=0x6c,
	KEYCODE_m=0x6d,
	KEYCODE_n=0x6e,
	KEYCODE_o=0x6f,
	KEYCODE_p=0x70,
	KEYCODE_q=0x71,
	KEYCODE_r=0x72,
	KEYCODE_s=0x73,
	KEYCODE_t=0x74,
	KEYCODE_u=0x75,
	KEYCODE_v=0x76,
	KEYCODE_w=0x77,
	KEYCODE_x=0x78,
	KEYCODE_y=0x79,
	KEYCODE_z=0x7a,

	KEYCODE_DELETE=0x7f,

	KEYCODE_RIGHT=0x1000,
	KEYCODE_LEFT=0x1001,
	KEYCODE_DOWN=0x1002,
	KEYCODE_UP=0x1003,

	KEYCODE_LSHIFT=0x1010,
	KEYCODE_RSHIFT=0x1011,
};


char Translate(uint8_t scancode, bool uppercase);
void handle_key(unsigned char keycode);

void set_keyboard_callback(void (*cb)(int,int));