#include "scheduler.h"
#include "thread.h"
#include "loop.h"
#include "pit.h"
#include "stdio.h"
#include "memory.h"
#include "fat.h"
#include "graphics.h"
#include <elf.h>
thread *current;
bool scheduler_inited=false;
extern void yield();
extern graphics_context* k_context;

void thread_function(){
	int thread_id = current->tid;
	while(1){
		printf("Thread %u",thread_id);
		//yield();
	}
}
void t3(){
	while(1){
		uint64_t time=(uint64_t)(TimeSinceBoot*100);

		printf("\033[s\033[30;0H%u %u %u\033[u",time,30,0);

		//sleep(1);
	}
}
void vsync(){
	while(1){
		//swap_buffer();
		draw_mouse(k_context->buf);
		sleep(100);
	}
}

thread *new_threads[256];
int num_threads=3;
uint64_t cur_thread=0;


thread* int_thread;
void start_scheduler(){
	printf("start_scheduler\n");
	new_threads[0]=new_thread(thread_function);
	//new_threads[1]=new_thread(t3);
	new_threads[1]=new_thread(thread_function);
	new_threads[2]=new_thread(thread_function);
	new_threads[3]=new_thread(thread_function);

	scheduler_inited=true;
	asm ("sti");
	loop();

}

bool first=true;
void* get_next_thread(void *stack_ptr){
	if(!scheduler_inited)
		return stack_ptr;
	if (first){
		first=false;
		current=new_threads[0];
		cur_thread=0;
		return current->RSP;
	}
	current->RSP=stack_ptr;
	cur_thread++;
	current=new_threads[cur_thread%num_threads];
	cur_thread%=num_threads;
	return current->RSP;
}

void new_process(char* executable,void* ptr){
	CHAR8* program_space=malloc(0x16000);

	printf("space allocated at,%x",program_space);

	CHAR8* program =  read_file(executable);

	Elf64_Ehdr* header=(Elf64_Ehdr*)program;
	printf("Arch: %u\n\n",(long)header->e_machine);
	for (int i = 0; i < 32; ++i)
	{
		long x=program[i];
		printf("%x,  ",x);
	}


	printf("entry: %u\n",header->e_entry);
	printf("ofset: %u\n",header->e_phoff);

	Elf64_Phdr* phdrs=(Elf64_Phdr*)(program+header->e_phoff);

	for (
		Elf64_Phdr* phdr = phdrs;
		(char*)phdr < (char*)phdrs + header->e_phnum * header->e_phentsize;
		phdr = (Elf64_Phdr*)((char*)phdr + header->e_phentsize)
	)
	{//load each program segment at the memory location indicated by its header in p_addr
		switch (phdr->p_type){
			case PT_LOAD:
			{
				int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
				Elf64_Addr segment = phdr->p_paddr;
				printf("Kernel p_paddr %u \n",phdr->p_paddr);

				//SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
				//map_mem(segment, REQUEST_PAGE());
				printf("%x",program_space +(uint64_t)segment);
				printf("%x",((uint64_t)segment));

				UINTN size = phdr->p_filesz;
				for (UINTN i = 0; i < size; ++i)
				{
					*(char*)(program_space+(uint64_t)segment+i)=program[phdr->p_offset+i];
				}
				break;
			}
		}
	}
	printf("Kernel e_entry %u \n",header->e_entry);
	printf("Kernel entry %u \n",&header->e_entry);


	int (*KernelStart)() = ((__attribute__((sysv_abi)) int (*)() ) program_space+header->e_entry);

	printf("Kernel start %u \n",KernelStart);
	KernelStart(ptr);
	//char* nel=calloc(1024);

	new_threads[num_threads++]=new_thread(thread_function);

}