#pragma once
#include "typedef.h"
#include "graphics.h"



#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39
#define Up_Arrow 0x48
#define Left_Arrow 0x4B
#define Right_Arrow 0x4D
#define Down_Arrow 0x50




char Translate(uint8_t scancode, bool uppercase);
void handle_key(unsigned char keycode);

