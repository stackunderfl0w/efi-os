void loop(){
	while(1){
		asm volatile("hlt");
	}
	//asm volatile ("1: jmp 1b");
}