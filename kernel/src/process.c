#include "process.h"
#include "sorted_list.h"
#include "memory.h"
#include "stdio.h"
#include "filesystem.h"
#include "thread.h"
#include "circular_buffer.h"
#include "elf.h"
extern circular_buffer* thread_pool;

dynarray* active_processes;

int cmp_process(void* first, void* second){
	return ((process*)first)->pid-((process*)second)->pid;
}
int search_process(void* first, void* second){
	return ((process*)first)->pid-(uint64_t)second;
}
void init_process_list(){
	active_processes = dynarray_init();
}

int next_pid=1;
void new_process(char* executable,void* ptr){
	uint8_t* program_space=kmalloc(0x16000);

	kprintf("space allocated at,%x\n",program_space);

	int exe=open(executable,0);

	struct stat st;
	fstat(exe,&st);
	uint8_t* program =  kmalloc(st.st_size);
	read(exe,program,st.st_size);

	close(exe);

	Elf64_Ehdr* header=(Elf64_Ehdr*)program;
	//kprintf("Arch: %u\n\n",(long)header->e_machine);



	kprintf("entry: %u\n",header->e_entry);
	kprintf("ofset: %u\n",header->e_phoff);

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
				//kprintf("prog p_paddr %u \n",phdr->p_paddr);
				//map_mem(segment, REQUEST_PAGE());
				//kprintf("%x",program_space +(uint64_t)segment);
				//kprintf("%x",((uint64_t)segment));

				UINTN size = phdr->p_filesz;
				for (UINTN i = 0; i < size; ++i){
					*(char*)(program_space+(uint64_t)segment+i)=program[phdr->p_offset+i];
				}
				break;
			}
		}
	}
	//kprintf("prog e_entry %u \n",header->e_entry);
	//kprintf("prog entry %u \n",&header->e_entry);


	void (*prog_entry)() = ((__attribute__((sysv_abi)) void (*)() ) program_space+header->e_entry);

	thread* t=new_thread(prog_entry,NULL);
	//user mode selectors
	((registers*)(t->RSP))->cs=0x20|3;
	((registers*)(t->RSP))->ss=0x28|3;
	((registers*)(t->RSP))->flags=0x2;

	//reg->cs=0x8;
	//reg->ss=0x10;

	cb_push(thread_pool,&t,1);
	((registers*)(t->RSP))->rdi=(uint64_t)ptr;

	process* p=kmalloc(sizeof(process));
	p->pid=next_pid++;
	//p->child_threads=dynarray_init();
	//p->working_dir=root
	//p->process_file_table
	//dynarray_add(active_processes,p);
}