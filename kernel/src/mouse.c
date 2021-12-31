#include "mouse.h"

uint32_t display_width=0,display_height=0;


void PS2_WAIT_WRITE(){
	uint32_t timeout=10000;
	while((inb(0x64)&0b10)&&timeout--){

	}
}
void PS2_WAIT_READ(){
	uint32_t timeout=10000;
	while((!inb(0x64)&0b1)&&timeout--){

	}
}
void PS2_WRITE_PORT_2(){
	PS2_WAIT_WRITE();
	outb(0x64,0xD4);
	PS2_WAIT_WRITE();
}
void INIT_PS2_MOUSE(){
	PS2_WAIT_WRITE();
	outb(0x64,0xA8);
	PS2_WAIT_WRITE();
	outb(0x64,0x20);
	PS2_WAIT_READ();
	uint8_t minp = inb(0x60);
	//print(to_hstring(minp));

	minp |= 1UL << 2;
	minp &= ~(1UL << 5);

	PS2_WAIT_WRITE();
	outb(0x64,0x60);
	PS2_WAIT_WRITE();
	outb(0x60,minp);

	PS2_WRITE_PORT_2();
	outb(0x60, 0xF6);

	PS2_WAIT_READ();
	inb(0x60);

	PS2_WRITE_PORT_2();
	outb(0x60, 0xF4);

	PS2_WAIT_READ();
	inb(0x60);
	get_display_resolution(&display_width, &display_height);

}

uint8_t fragment_counter=0;
uint8_t fragments[4];
int mouse_x=0,mouse_y=0;

#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000100
#define PS2Rightbutton 0b00000010
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

//bool fr=true;
char fr=2;
void HANDLE_PS2_MOUSE(uint8_t frag){
	if(fr){
		fr--;
		return;
	}
	fragments[fragment_counter]=frag;
	fragment_counter++;
	if(fragment_counter==3){
		fragment_counter=0;
		bool x_neg, y_neg, x_over, y_over;

		x_neg=fragments[0] & X_SIGN;
		y_neg=fragments[0] & Y_SIGN;
		x_over=fragments[0] & X_OVERFLOW;
		y_over=fragments[0] & Y_OVERFLOW;

		if(x_neg){
			mouse_x-=256-fragments[1];
		}
		else{
			mouse_x+=fragments[1];
		}

		if(y_neg){
			mouse_y+=256-fragments[2];
		}
		else{
			mouse_y-=fragments[2];
		}

		if(mouse_x < 0) mouse_x = 0;
		if(mouse_x > display_width-1) mouse_x = display_width-1;

		if(mouse_y < 0) mouse_y = 0;
		if(mouse_y > display_height-1) mouse_y = display_height-1;


		clear_mouse();
		if (fragments[0]&LEFT_BTN){
			putchar(mouse_x,mouse_y,'a');
		}
		move_mouse(mouse_x,mouse_y);
	}
}