#pragma once
#include "typedef.h"


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
	KEYCODE_BACKSPACE='\b',
	KEYCODE_TAB='\t',
	KEYCODE_RETURN='\n',

	KEYCODE_SPACE=' ',

	KEYCODE_EXCLAIM='!',
	KEYCODE_QUOTEDBL='"',
	KEYCODE_HASH='#',
	KEYCODE_DOLLAR='$',
	KEYCODE_PERCENT='%',
	KEYCODE_AMPERSAND='&',
	KEYCODE_QUOTE='\'',
	KEYCODE_LEFTPAREN='(',
	KEYCODE_RIGHTPAREN=')',
	KEYCODE_ASTERISK='*',
	KEYCODE_PLUS='+',
	KEYCODE_COMMA=',',
	KEYCODE_MINUS='-',
	KEYCODE_PERIOD='.',
	KEYCODE_SLASH='/',

	KEYCODE_0='0',
	KEYCODE_1='1',
	KEYCODE_2='2',
	KEYCODE_3='3',
	KEYCODE_4='4',
	KEYCODE_5='5',
	KEYCODE_6='6',
	KEYCODE_7='7',
	KEYCODE_8='8',
	KEYCODE_9='9',

	KEYCODE_COLON=':',
	KEYCODE_SEMICOLON=';',
	KEYCODE_LESS='<',
	KEYCODE_EQUALS='=',
	KEYCODE_GREATER='>',
	KEYCODE_QUESTION='?',
	KEYCODE_AT='@',

	KEYCODE_LEFTBRACKET='[',
	KEYCODE_BACKSLASH='\\',
	KEYCODE_RIGHTBRACKET=']',
	KEYCODE_CARET='^',
	KEYCODE_UNDERSCORE='_',

	KEYCODE_BACKQUOTE='`',

	KEYCODE_a='a',
	KEYCODE_b='b',
	KEYCODE_c='c',
	KEYCODE_d='d',
	KEYCODE_e='e',
	KEYCODE_f='f',
	KEYCODE_g='g',
	KEYCODE_h='h',
	KEYCODE_i='i',
	KEYCODE_j='j',
	KEYCODE_k='k',
	KEYCODE_l='l',
	KEYCODE_m='m',
	KEYCODE_n='n',
	KEYCODE_o='o',
	KEYCODE_p='p',
	KEYCODE_q='q',
	KEYCODE_r='r',
	KEYCODE_s='s',
	KEYCODE_t='t',
	KEYCODE_u='u',
	KEYCODE_v='v',
	KEYCODE_w='w',
	KEYCODE_x='x',
	KEYCODE_y='y',
	KEYCODE_z='z',

	KEYCODE_DELETE='\x7F',

	KEYCODE_RIGHT=0x1000,
	KEYCODE_LEFT=0x1001,
	KEYCODE_DOWN=0x1002,
	KEYCODE_UP=0x1003,

	KEYCODE_LSHIFT=0x1010,
	KEYCODE_RSHIFT=0x1011,
};

enum modifiers{
	MODCODE_LSHIFT=1,
	MODCODE_RSHIFT=1<<1,
	MODCODE_SHIFT=MODCODE_RSHIFT|MODCODE_LSHIFT,
	MODCODE_KEYDOWN=1<<2,
	MODCODE_KEYUP=1<<3,

};

typedef struct{
	int keycode;
	int modifiers;
}key_event;

char Translate(uint8_t scancode, bool uppercase);
void handle_key(unsigned char keycode);

void set_keyboard_callback(void (*cb)(int,int));