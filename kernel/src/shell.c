#include "shell.h"
#include "fat.h"
#include "stdio.h"
#include "ctype.h"
#include "scheduler.h"
#include "paging.h"
#include "filesystem.h"
#include "memory.h"
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
		kprintf("%c\033[D",0x18);
		cursor_present=true;
		//fflush(stdout);
	}
}
void hide_cursor(){
	if(cursor_present){
		kprintf(" \033[D");
		//fflush(stdout);
		cursor_present=false;
	}
}

void print_character(char keycode){
	if(isLeftShift||isRightShift){
		if (isalpha(keycode)){
			kprintf("%c",keycode-32);
		}
		else if(isdigit(keycode)){
			kprintf("%c",num_shift[keycode-0x30]);
		}
	}
	else{
		kprintf("%c",keycode);
	}
}
void new_console_line(){
	kprintf("\n[%s@%s: ~]$",username,hostname);
	kprintf("\033[s\nhello1\033[uhello2\n");

}

void run_shell(Framebuffer* buf, bitmap_font* font){
	//kprintf("\n[%s@%s: ~]$",username,hostname);
	char cmd[1024];

	int index=0;
	char c=-1;

	bool running=true;
	cursor_active=true;
	char pwd_buf[1024];


	while(running){
		//kprintf("[%s@%s: ~]$",username,hostname);
		getcwd(pwd_buf,1024);

		kprintf("[%s@%s: %s]$",username,hostname,pwd_buf);
		//chdir("resources/resourcesresources");
		index=0;
		memset(cmd,0,1024);
		/*while(cmd[index-1]!=10){
			if(c=fgetc(stdin)){
				cmd[index++]=c;
				if(c==10){
				}
				//kprintf("%u",c);

			}
			else{
				yield();
			}
		}
		cmd[index-1]=0;

		run_cmd(cmd);*/
		//sleep(500);
		//cursor_flash=true;
		//show_cursor();
		//sleep(500);
		//hide_cursor();
	}
}
void run_cmd(char* cmd){
	int argc;
	char** argv=split_string_by_char(cmd,' ',&argc);
	cursor_active=false;
	if(cursor_present){
		cursor_present=false;
		kprintf("\033[D ");
	}
	if(!strcmp(argv[0],"free")){
		kprintf("Free memory: %ukb\n",get_free_memory()/1024);
		kprintf("Used memory: %ukb\n",get_used_memory()/1024);
		kprintf("Reserved memory: %ukb\n",get_reserved_memory()/1024);
	}
	if(!strcmp(argv[0],"cd")){
		if(argc>1){
			if(chdir(argv[1])==-1){
				kprintf("sh: cd: %s: No such file or directory",argv[1]);
			}
		}
	}
	else if(!strcmp(argv[0],"ls")){
		int entries;
		//char** files=read_directory("/",&entries);
		
	}

	free(argv);


	cursor_active=true;
}
