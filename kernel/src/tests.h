#pragma once
#include "graphics.h"
#include "serial.h"
#include "stdio.h"
#include "fat.h"
#include "pit.h"

void test_ascii_colors(){
			printf("\033[30m black\n"
			"\033[31m red\n"
			"\033[32m Green\n"
			"\033[33m Yellow\n"
			"\033[34m Blue\n"
			"\033[35m Magenta\n"
			"\033[36m Cyan\n"
			"\033[37m White\n");
		printf("\033[40m black\n"
			"\033[41m red\n"
			"\033[42m Green\n"
			"\033[43m Yellow\n"
			"\033[44m Blue\n"
			"\033[45m Magenta\n"
			"\033[46m Cyan\n"
			"\033[47m White\n");
		printf("\033[31;40m black\n"
			"\033[32;41m red\n"
			"\033[33;42m Green\n"
			"\033[34;43m Yellow\n"
			"\033[35;44m Blue\n"
			"\033[36;45m Magenta\n"
			"\033[37;46m Cyan\n"
			"\033[30;47m White\n");
}
void test_ata_pio_read(){
	uint8_t* sector_1=malloc(512);;

	//atapio_software_reset(ATAPIO_REGULAR_STATUS_REGISTER_PORT);

	atapio_read_sectors(0, 1, sector_1);

	for (int i = 0; i < 512; ++i){
		uint64_t tmp=sector_1[i];
		printf("%x ",tmp);
	}
	atapio_read_sectors(1, 1, sector_1);

	for (int i = 0; i < 512; ++i){
		uint64_t tmp=sector_1[i];
		printf("%x ",tmp);
	}
}

void test_print_char(){
	for (int i = 0; i < 256; ++i){
		printf("%c",i);
	}
}
void test_interupts(){
	asm("int $0x0e");

}
void test_serial(){
	print_serial("hello world");
}

void test_benchmark_clrscr(){
	for (int i = 0; i < 10000; ++i){
		//clrscr(i*100);
		//memset(info->buf->BaseAddress,i%100,info->buf->PixelsPerScanLine*info->buf->Height*4);
		//wmemset(buf->BaseAddress,i*100,buf->PixelsPerScanLine*buf->Height*2);
		//intset(info->buf->BaseAddress,i*100,info->buf->PixelsPerScanLine*info->buf->Height);
	}
}

void test_heap(){
	void* test_mal=calloc(0x200);
	printf("\n%p\n",test_mal);

	void* test_mal2=calloc(0x10);
	printf("\n%p\n",test_mal2);
	free(test_mal);

	void* test_mal3=calloc(0x20);
	printf("\n%p\n",test_mal3);

	void* test_mal4=calloc(0x10000);
	printf("\n%p\n",test_mal4);

	free(test_mal2);
	free(test_mal3);
	free(test_mal4);
	void* test_mal5=calloc(0x1000);
	printf("\n%p\n",test_mal5);

	double starttime=TimeSinceBoot;

	for (int i = 0; i < 1000; ++i){
		void* tmp =malloc(0x100);
	}
	double time_to_finish=TimeSinceBoot-starttime;

	uint64_t time=(uint64_t)(TimeSinceBoot*100);
	printf("%u\n",time);
	printf("test realoc");
	char* chr_ptr=malloc(13);
	char* src="Hello there";
	memcpy(chr_ptr,src,12);
	printf(chr_ptr);
	realloc(chr_ptr,256);
	printf(chr_ptr);
}

void test_paging(){
	for (int i = 0; i < 6; ++i){
		void* test = REQUEST_PAGE();
		printf("%u\n",test);
	}

	map_mem((void*)0x6000000000,REQUEST_PAGE());

	uint64_t* test =(uint64_t*)0x6000000000;

	*test=30000;

	printf("%u\n",*test);


	void* test_ptr = REQUEST_PAGE();
	printf("%p\n",test_ptr);
}

void test_benchmark_paging(){
	double starttime=TimeSinceBoot;

	for (int i = 0; i < 50000; ++i){
		REQUEST_PAGE();
	}

	double time_to_finish=TimeSinceBoot-starttime;

	uint64_t time=(uint64_t)(TimeSinceBoot*100);
	printf("%u\n",time);
}
void test_printf(){
	printf("test printf\n");

	printf("hello\n");

	printf("hi %x\n",0x12345);
	printf("hi %u \n %x \n",7812345,0x55);
	double doub= 12345.6789;

	printf("%f",doub);

	printf("double %f kk\nff",doub);
	printf("\033[s\nhello1\033[uhello2\n");
}
void test_put(){
	fputc(10, stdout);
	fputs("eejfaousdnawndoianwdian",stdout);
}