#include "loop.h"
void loop(){
	while(1){
		asm volatile("hlt");
	}
	//asm volatile ("1: jmp 1b");
}
void busyloop(uint64_t count){
	for (uint64_t i = 0; i < count; ++i){
		i++;
	}
}