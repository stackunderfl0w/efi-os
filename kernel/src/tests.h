#pragma once
#include "graphics.h"
#include "fat.h"

void test_ata_pio_read(){
	uint8_t* sector_1=malloc(512);;

	//atapio_software_reset(ATAPIO_REGULAR_STATUS_REGISTER_PORT);

	atapio_read_sectors(0, 1, sector_1);

	for (int i = 0; i < 512; ++i){
		uint64_t tmp=sector_1[i];
		print(to_hstring_noformat(tmp));
		printchar(' ');
	}
	atapio_read_sectors(1, 1, sector_1);

	for (int i = 0; i < 512; ++i){
		uint64_t tmp=sector_1[i];
		print(to_hstring_noformat(tmp));
		printchar(' ');
	}p
}

void test_print_char(){
	for (int i = 0; i < 256; ++i){
		printchar(i);
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
		clrscr(i*100);
		//memset(info->buf->BaseAddress,i%100,info->buf->PixelsPerScanLine*info->buf->Height*4);
		//wmemset(buf->BaseAddress,i*100,buf->PixelsPerScanLine*buf->Height*2);
		//intset(info->buf->BaseAddress,i*100,info->buf->PixelsPerScanLine*info->buf->Height);
	}
}

void test_heap(){
	void* test_mal=calloc(0x200);
	printchar('\n');
	print(to_hstring((uint64_t)test_mal));

	void* test_mal2=calloc(0x10);
	printchar('\n');
	print(to_hstring((uint64_t)test_mal2));
	free(test_mal);

	void* test_mal3=calloc(0x20);
	printchar('\n');
	print(to_hstring((uint64_t)test_mal3));


	void* test_mal4=calloc(0x10000);
	printchar('\n');
	print(to_hstring((uint64_t)test_mal4));


	free(test_mal2);
	free(test_mal3);
	free(test_mal4);
	void* test_mal5=calloc(0x1000);
	printchar(' ');
	print(to_hstring((uint64_t)test_mal5));
	printchar(' ');

	double starttime=TimeSinceBoot;

	for (int i = 0; i < 1000; ++i){
		void* tmp =malloc(0x100);
	}
	double time_to_finish=TimeSinceBoot-starttime;

	time=(uint64_t)(TimeSinceBoot*100);
	print(to_string(time));
	printchar('\n');
	print("test realoc")
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
		print(to_hstring((uint64_t)test));
		printchar('\n');
	}

	map_mem((void*)0x6000000000,REQUEST_PAGE());

	uint64_t* test =(uint64_t*)0x6000000000;

	*test=30000;

	print(to_string(*test));
	printchar('\n');


	void* test_ptr = REQUEST_PAGE();
	print(to_hstring((uint64_t)test_ptr));


	printchar('\n');
}

void test_benchmark_paging(){

	double starttime=TimeSinceBoot;

	for (int i = 0; i < 50000; ++i){
		REQUEST_PAGE();
	}

	double time_to_finish=TimeSinceBoot-starttime;

	time=(uint64_t)(TimeSinceBoot*100);
	print(to_string(time));
	printchar('\n');
}
void test_printf(){
	print("test printf\n");

	printf("hello\n");

	printf("hi %x\n",0x12345);
	printf("hi %u \n %x \n",7812345,0x55);
	double doub= 12345.6789;

	print(to_string_double(doub,3));

	printf("double %f kk\nff",doub);

}
//asm ("movq %%rsp, %0" : "=r" (i) );
