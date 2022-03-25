#include "time.h"

volatile time SYSTEM_TIME;
#define CMOS_COMMAND 	0x70
#define CMOS_DATA 		0x71

int get_update_in_progress_flag() {
	outb(CMOS_COMMAND, 0x0A);
	return inb(CMOS_DATA) & 0x80;
}

uint8_t READ_RTC_REGISTER(uint16_t reg) {
	outb(CMOS_COMMAND, reg);
	return inb(CMOS_DATA);
}

uint8_t bcd_to_int(uint8_t bcd){
	return (bcd&0x0F)+((bcd/16)*10);
}



void INIT_RTC(){
	//wait for update to finish
	while (get_update_in_progress_flag());

	uint8_t registerB = READ_RTC_REGISTER(0x0B);

	uint8_t second = READ_RTC_REGISTER(0x00);
	uint8_t minute = READ_RTC_REGISTER(0x02);
	uint8_t hour = READ_RTC_REGISTER(0x04);
	uint8_t day = READ_RTC_REGISTER(0x07);
	uint8_t month = READ_RTC_REGISTER(0x08);
	uint8_t year = READ_RTC_REGISTER(0x09);


	if (!(registerB&0x04)){
		//print("bcd");
		second=bcd_to_int(second);
		minute=bcd_to_int(minute);
		hour=bcd_to_int(hour);
		day=bcd_to_int(day);
		month=bcd_to_int(month);
		year=bcd_to_int(year);

	}
	SYSTEM_TIME.second=second;
	SYSTEM_TIME.minute=minute;
	SYSTEM_TIME.hour=hour;
	SYSTEM_TIME.day=day;
	SYSTEM_TIME.month=month;
	SYSTEM_TIME.year=2000+year;

	if(SYSTEM_TIME.hour>8){
		SYSTEM_TIME.hour-=8;
	}
	else{
		SYSTEM_TIME.hour+=16;
		SYSTEM_TIME.day-=1;
	}


	//printf("second:%u\nminute:%u\nhour:%u\nday:%u\nmonth:%u\nyear:%u\n",
		//SYSTEM_TIME.second,SYSTEM_TIME.minute,SYSTEM_TIME.hour,SYSTEM_TIME.day,SYSTEM_TIME.month,SYSTEM_TIME.year);

	//printf("%u %u %u", SYSTEM_TIME.year, SYSTEM_TIME.month, SYSTEM_TIME.day);

}
void SYSTEM_TIME_INCREMENT(){
	SYSTEM_TIME.second++;
	if(SYSTEM_TIME.second>=60){
		SYSTEM_TIME.second=0;
		SYSTEM_TIME.minute++;
	}
	if(SYSTEM_TIME.minute>=60){
		SYSTEM_TIME.minute=0;
		SYSTEM_TIME.hour++;
	}
	if(SYSTEM_TIME.hour>=24){
		SYSTEM_TIME.hour=0;
		SYSTEM_TIME.day++;
	}
	if(SYSTEM_TIME.day>=60){
		SYSTEM_TIME.day=0;
		SYSTEM_TIME.month++;
	}
	if(SYSTEM_TIME.month>=60){
		SYSTEM_TIME.month=0;
		SYSTEM_TIME.year++;
	}
}
uint64_t dayofweek(){      /* 0 = Sunday */
	int y=SYSTEM_TIME.year;
	int m=SYSTEM_TIME.month;
	int d=SYSTEM_TIME.day;
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}
const char* days_of_the_week[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
const char* months[]={"January","Febuary","March","April","May","June","July","August","September","October","November","December"};
const char* days_of_the_week_short[]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
const char* months_short[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
