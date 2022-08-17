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
extern graphics_context* k_context;
extern graphics_context* global_context;

void thread_function(){
	int thread_id = current->tid;
	while(1){
		printf("Thread %u %f\n",thread_id,TimeSinceBoot);
		sleep(10);
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
	double last_frame[60];
	char st[32];
	double fps;
	int count=0;
	while(1){
		for (int i = 59; i > 0; --i){
			last_frame[i]=last_frame[i-1];
		}
		last_frame[0]=TimeSinceBoot;
		swap_buffer(global_context->buf,k_context->buf);
		int x=global_context->cursor_x,y=global_context->cursor_y;
		global_context->cursor_x=10;
		global_context->cursor_y=global_context->console_height-1;
		count++;
		if (count>10){
			count=0;
			//fps =(1/((last_frame[0]-last_frame[59]))*(60-1));
			fps =1/(last_frame[0]-last_frame[59])*59;
			memset(st,0,32);
			sprintf(st,"%f",fps);
		}
		print(global_context,st);
	}
}

thread *new_threads[256];
int num_threads=2;
uint64_t cur_thread=0;


thread* int_thread;
void start_scheduler(){
	asm("cli");
	printf("start_scheduler\n");
	//new_threads[0] is base thread
	//new_threads[1]=new_thread(thread_function);
	//new_threads[2]=new_thread(thread_function);
	//new_threads[3]=new_thread(thread_function);
	new_threads[1]=new_thread(vsync);

	//new_process("/resources/scrclr.elf", k_context->buf->BaseAddress);

	scheduler_inited=true;
	asm("sti");
	//loop();

}

bool first=true;
void* get_next_thread(void *stack_ptr){
	if(!scheduler_inited)
		return stack_ptr;
	if (first){
		first=false;
		current=new_threads[0];
		cur_thread=0;
		return stack_ptr;
	}
	current->RSP=stack_ptr;
	cur_thread++;
	current=new_threads[cur_thread%num_threads];
	cur_thread%=num_threads;
	return current->RSP;
}

void new_process(char* executable,void* ptr){
	CHAR8* program_space=malloc(0x16000);

	printf("space allocated at,%x\n",program_space);

	CHAR8* program =  read_file(executable);

	Elf64_Ehdr* header=(Elf64_Ehdr*)program;
	/*printf("Arch: %u\n\n",(long)header->e_machine);
	for (int i = 0; i < 32; ++i)
	{
		long x=program[i];
		printf("%x,  ",x);
	}*/


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
				//printf("prog p_paddr %u \n",phdr->p_paddr);

				//SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
				//map_mem(segment, REQUEST_PAGE());
				//printf("%x",program_space +(uint64_t)segment);
				//printf("%x",((uint64_t)segment));

				UINTN size = phdr->p_filesz;
				for (UINTN i = 0; i < size; ++i){
					*(char*)(program_space+(uint64_t)segment+i)=program[phdr->p_offset+i];
				}
				break;
			}
		}
	}
	//printf("prog e_entry %u \n",header->e_entry);
	//printf("prog entry %u \n",&header->e_entry);


	void (*prog_entry)() = ((__attribute__((sysv_abi)) void (*)() ) program_space+header->e_entry);

	//printf("prog start %u \n",prog_entry);
	//prog_entry(ptr);

	new_threads[num_threads++]=new_thread(prog_entry);
	((registers*)(new_threads[num_threads-1]->stack_ptr))->rdi=(uint64_t)ptr;

}