#pragma once
#include "typedef.h"
#include "io.h"
#include "graphics.h"


void PS2_WAIT_WRITE();

void PS2_WAIT_READ();

void PS2_WRITE_PORT_2();

void INIT_PS2_MOUSE();

void HANDLE_PS2_MOUSE(uint8_t frag);

#define LEFT_BTN	0b00000001
#define RIGHT_BTN	0b00000010
#define MIDDLE_BTN	0b00000100
#define ALWAYS_1	0b00001000
#define X_SIGN		0b00010000
#define Y_SIGN		0b00100000
#define X_OVERFLOW	0b01000000
#define Y_OVERFLOW	0b10000000