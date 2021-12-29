#include "graphics.h"

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