#include "pit.h"
#include "scheduler.h"
#include "io.h"
#include "time.h"

volatile double TimeSinceBoot = 0;
uint16_t Divisor = 65535;
const uint64_t BaseFrequency = 1193182;
//const uint64_t BaseFrequency = 1193400;
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

void SET_PIT_DIVISOR(uint16_t divisor){
	//if (divisor < 100) divisor = 100;
	divisor=max(divisor,100);
	Divisor = divisor;
	outb(0x40, (uint8_t)(divisor & 0x00ff));
	outb(0x40, (uint8_t)(divisor >> 8));
}
void SET_PIT_FREQUENCY(uint64_t Frequency){
	SET_PIT_DIVISOR(BaseFrequency/Frequency);
}
double GET_PIT_FREQUENCY(){
	return (double)BaseFrequency / (double)Divisor;
}
void PIT_TICK(){
	volatile uint64_t old_time=(uint64_t)TimeSinceBoot;
	TimeSinceBoot+=1.01/GET_PIT_FREQUENCY();
	if((uint64_t)TimeSinceBoot>old_time){
		SYSTEM_TIME_INCREMENT();
	}
}

void sleep(uint64_t miliseconds){
	volatile double end=TimeSinceBoot+((double)miliseconds)/1000;
	while(TimeSinceBoot<end){
		if(scheduler_inited)
			yield();
		else
			asm("hlt");
	}
}