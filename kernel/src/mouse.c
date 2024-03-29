#include "mouse.h"
extern graphics_context* current_context;

uint32_t display_width=0,display_height=0;


void PS2_WAIT_WRITE(uint16_t port, uint8_t data){
	uint32_t timeout=10000;
	while((inb(0x64)&0b10)&&timeout--);
	outb(port,data);
}
uint8_t PS2_WAIT_READ(uint16_t port){
	uint32_t timeout=10000;
	while((!inb(0x64)&0b1)&&timeout--);
	return inb(port);
}
void PS2_WRITE_PORT_2(uint16_t port, uint8_t data){
	PS2_WAIT_WRITE(0x64,0xD4);
	outb(port,data);
}
void INIT_PS2_MOUSE(){
	PS2_WAIT_WRITE(0x64,0xA8);
	PS2_WAIT_WRITE(0x64,0x20);

	uint8_t minp = PS2_WAIT_READ(0x60)|2;

	PS2_WAIT_WRITE(0x64,0x60);
	PS2_WAIT_WRITE(0x60,minp);

	PS2_WRITE_PORT_2(0x60, 0xF6);

	PS2_WAIT_READ(0x60);

	PS2_WRITE_PORT_2(0x60, 0xF4);

	PS2_WAIT_READ(0x60);
	
	get_display_resolution(current_context->buf,&display_width, &display_height);
}

uint8_t fragment_counter=0;
uint8_t fragments[4];
int mouse_x=0,mouse_y=0;

#define PS2Leftbutton		0b00000001
#define PS2Middlebutton		0b00000100
#define PS2Rightbutton		0b00000010
#define PS2XSign			0b00010000
#define PS2YSign			0b00100000
#define PS2XOverflow		0b01000000
#define PS2YOverflow		0b10000000

//bool fr=true;
char skip=1;
void HANDLE_PS2_MOUSE(uint8_t frag){
	if(skip){
		skip--;
		return;
	}
	fragments[fragment_counter++]=frag;
	if(fragment_counter==3){
		fragment_counter=0;
		bool x_neg, y_neg;

		x_neg=fragments[0] & PS2XSign;
		y_neg=fragments[0] & PS2YSign;
		//discard on overflow
		if(fragments[0]&(PS2XOverflow|PS2YOverflow))
			return;

		mouse_x+=x_neg?fragments[1]-256:fragments[1];
		mouse_y-=y_neg?fragments[2]-256:fragments[2];
		
		mouse_x=MIN(MAX(mouse_x,0),display_width-1);
		mouse_y=MIN(MAX(mouse_y,0),display_height-1);

		//clear_mouse();
		if (fragments[0]&PS2Leftbutton){
			render_char(current_context,mouse_x,mouse_y,'a');
		}
		move_mouse(mouse_x,mouse_y);
	}
}