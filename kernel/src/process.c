#include "process.h"
#include "memory.h"
#include "stdio.h"
#include "filesystem.h"
#include "thread.h"
#include "circular_buffer.h"
#include "elf.h"
extern circular_buffer* thread_pool;

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

				//SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
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

	thread* t=new_thread(prog_entry);
	cb_push(thread_pool,&t,1);
	((registers*)(t->RSP))->rdi=(uint64_t)ptr;
}