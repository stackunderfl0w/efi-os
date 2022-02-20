#pragma once
#include "typedef.h"
#include "io.h"
//#include "graphics.h"

typedef struct{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint32_t year;
}time;

enum days{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
};

extern const char* days_of_the_week[7];
extern const char* months[12];
extern const char* days_of_the_week_short[7];
extern const char* months_short[12];

extern volatile time SYSTEM_TIME;
#define CMOS_COMMAND 	0x70
#define CMOS_DATA 		0x71


void INIT_RTC();
void SYSTEM_TIME_INCREMENT();
uint64_t dayofweek();