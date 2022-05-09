#include "shell.h"
#include "fat.h"
#include "stdio.h"

bool is_alpha(char ch){
	return((ch >= 97 && ch <= 122) || (ch >= 65 && ch <= 90));
}
bool is_print(char ch){
	return((ch >= 0x20 && ch <= 0x7e));
}
bool isdigit(char ch){
	return ((ch>=0x30 && ch<=0x39));
}
void run_cmd(char* cmd);

char num_shift[10]={')','!','@','#','$','%','^','&','*','('};
char cmd[4096]={0};
int cmd_index=0;
bool cursor_present=false;
bool cursor_flash=false;
bool cursor_active=false;
bool isLeftShift=false;
bool isRightShift=false;
char* username="pat";
char* hostname="efios";

void show_cursor(){
	if(cursor_flash&&!cursor_present){
		printchar(0x18);
		cursor_present=true;
	}
}
void hide_cursor(){
	if(cursor_present){
		deletechar();
		cursor_present=false;
	}
}

void print_character(char keycode){
	if(isLeftShift||isRightShift){
		if (is_alpha(keycode)){
			printchar(keycode-32);
		}
		else if(isdigit(keycode)){
			printchar(num_shift[keycode-0x30]);
		}
	}
	else{
		printchar(keycode);
	}
}
void new_console_line(){
	printchar('\n');
	printf("[%s@%s: ~]$",username,hostname);
}
void kb_callback(int keycode, int action){
	//printf("hi\n");
	if (keycode!=0){
		//printchar(keycode);
		//printf("%c %s\n",keycode,action==PRESS?"PRESSED":"UNPRESSED");
	}
	if(keycode==KEYCODE_LSHIFT){
		isLeftShift=!(bool)action;
	}
	if(keycode==KEYCODE_RSHIFT){
		isRightShift=!(bool)action;
	}
	if(action==PRESS){
		if(is_print(keycode)){
			hide_cursor();
			print_character(keycode);
			show_cursor();
			cmd[cmd_index++]=keycode;
		}
		if(keycode==KEYCODE_BACKSPACE){
			if(cmd_index>0){
				hide_cursor();
				deletechar();
				show_cursor();
				cmd[--cmd_index]=0;
			}
		}
		if(keycode==KEYCODE_RETURN){
			hide_cursor();
			printchar('\n');
			printf("%s\n",cmd);
			run_cmd(cmd);
			memset(cmd,0,4096);
			cmd_index=0;
			new_console_line();
			show_cursor();
		}
	}
}


void run_shell(Framebuffer* buf, bitmap_font* font){
	init_text_overlay(buf, font);
	printf("[%s@%s: ~]$",username,hostname);
	set_keyboard_callback(kb_callback);

	bool running=true;
	cursor_active=true;
	while(running){
		sleep(500);
		cursor_flash=true;
		show_cursor();
		sleep(500);
		hide_cursor();
	}
}
void run_cmd(char* cmd){
	cursor_active=false;
	if(cursor_present){
		cursor_present=false;
		deletechar();
	}
	if(!strcmp(cmd,"free")){
		printf("Free memory: %ukb\n",get_free_memory()/1024);
		printf("Used memory: %ukb\n",get_used_memory()/1024);
		printf("Reserved memory: %ukb\n",get_reserved_memory()/1024);
	}
	else if(!strcmp(cmd,"ls")){
		int entries;
		char** files=read_directory("/",&entries);
		
	}


	cursor_active=true;
}
