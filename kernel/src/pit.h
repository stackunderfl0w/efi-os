#pragma once
#include "typedef.h"
#include "io.h"
#include "graphics.h"
#include "time.h"
volatile extern double TimeSinceBoot;

void SET_PIT_DIVISOR(uint16_t divisor);
void SET_PIT_FREQUENCY(uint64_t Frequency);
void PIT_TICK();
void sleep(uint64_t miliseconds);
