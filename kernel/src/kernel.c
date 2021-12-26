#include "typedef.h"
#include "bitmap-font.h"
#include "graphics.h"
#include "gdt.h"
//#include "loop.h"
#include "idt.h"
#include "pit.h"
#include "efimemory.h"
#include "mouse.h"
#include "paging.h"
#include "memory.h"
#include "serial.h"
#include "ata-pio.h"
#include "fat.h"

typedef struct {
	Framebuffer* buf;
	bitmap_font* font;
	//void* psf1;
	EFI_MEMORY_DESCRIPTOR* mem_map;
	UINTN map_size;
	UINTN map_desc_size;
}bootinfo;

#ifndef __cplusplus
#include "loop.h"

#else
extern "C" {
#include "loop.h"
}

extern "C" 
#endif
int _start(bootinfo *info){

	//init_serial();

	init_text_overlay(info->buf, info->font);
	//print_serial("hello world");

	//gdt
	print("didnt crash ");
	INIT_GDT();
	print("GDT loaded ");

	create_interrupts();

	//asm("int $0x0e");

	print("idt loaded ");
	//asm volatile ("1: jmp 1b");

	//INIT_PS2_MOUSE();
	/*for (int i = 0; i < 1000; ++i)
	{
		printchar(i%256);
		deletechar();
	}*/
	//SET_PIT_DIVISOR(65535);
	SET_PIT_FREQUENCY(1000);


	//asm("cli");
	init_text_overlay(info->buf, info->font);
	//asm("sti");



	uint32_t numEntries=info->map_size/info->map_desc_size;


	printchar('\n');
    INIT_PS2_MOUSE();

	asm ("sti");

    print("mouse inited");
    print(to_hstring((uint64_t)info->buf->BaseAddress));

	uint64_t memsize=getMemorySize(info->mem_map,numEntries,info->map_desc_size);
	printchar('\n');

	print(to_string(memsize));
	printchar('\n');

	print("Initializing paging\n");
    INIT_PAGING(info->mem_map,numEntries,info->map_desc_size,info->buf);


    print("page table loaded\n");

    printf("Free memory: %ukb\n",get_free_memory()/1024);
    printf("Used memory: %ukb\n",get_used_memory()/1024);
    printf("Reserved memory: %ukb\n",get_reserved_memory()/1024);


    INIT_HEAP((void*)0x8000000000,0x10);



    //unsigned char sector_1[512];
    unsigned char* sector_1=malloc(512);;

	atapio_software_reset(ATAPIO_REGULAR_STATUS_REGISTER_PORT);

	atapio_read_sectors(0, 1, sector_1);

    /*for (int i = 0; i < 512; ++i)
    {
    	uint64_t tmp=sector_1[i];
    	print(to_hstring_noformat(tmp));
    	printchar(' ');
    }*/
	if (sector_1[0]==0xEB&&sector_1[1]==0x3C&&sector_1[2]==0x90){
		print("Fat fileystem found ");
		char oem[9];
		memcpy(oem,&sector_1[3],8);
		oem[8]=0;
		print(oem);
		printchar('\n');
		print(to_string((uint64_t)*(uint16_t*)&sector_1[0xb]));
		print(to_string((uint64_t)sector_1[0xd]));
		printchar('\n');

		fat_BS* fat = (fat_BS*)sector_1;
		printf("Bytes per Sector:%u\n",fat->bytes_per_sector);
		printf("Sectors per Cluster:%u\n",fat->sectors_per_cluster);
		printf("Reserved sectors:%u\n",fat->reserved_sector_count);
		printf("Fat tables:%u\n",fat->table_count);
		printf("Root entry count:%u\n",fat->root_entry_count);
		printf("Total sectors:%u\n",fat->total_sectors_16);
		printf("Media Type:%x\n",fat->media_type);
		printf("Table size:%u\n",fat->table_size_16);
		printf("Sectors per track:%u\n",fat->sectors_per_track);
		printf("Head side count:%u\n",fat->head_side_count);
		printf("Hidden sectors:%u\n",fat->hidden_sector_count);
		printf("Total sectors:%u\n",fat->total_sectors_32);



		printf("Root dir %u %x\n",get_first_root_dir_sector(fat),512*get_first_root_dir_sector(fat));

		uint8_t* root_dir_entry=malloc(512);
		uint8_t* first_fat_table=malloc(512);

		atapio_read_sectors(get_first_root_dir_sector(fat), 1, root_dir_entry);
		atapio_read_sectors(get_first_fat_sector(fat),1,first_fat_table);
		char tmp_name[256];
		char tmp_long[16];
		for (uint64_t i = 0; root_dir_entry[i]; i+=32)
		{
			if(root_dir_entry[i]==0xE5){
				continue;
			}
			if(root_dir_entry[i+11]==0xf){
				FAT_LONG_NAME_ENTRY* lfn=&root_dir_entry[i];
				print("lfn: ");
				memset(tmp_long,0,16);
				for (int i = 0; i < 5; ++i)
				{
					tmp_long[i]=lfn->first_5[i]&0xff;
				}
				for (int i = 0; i < 6; ++i)
				{
					tmp_long[i+5]=lfn->next_6[i]&0xff;
				}
				for (int i = 0; i < 2; ++i)
				{
					tmp_long[i+11]=lfn->last_2[i]&0xff;
				}
				tmp_long[14]=0;
				print(tmp_long);
			}
			else{
				FAT_DIRECTORY_ENTRY* entry=&root_dir_entry[i];
				memcpy(tmp_name,entry->name,11);
				tmp_name[11]=0;
				print(tmp_name);
				printf("Cluster:%u at:%x\n",entry->first_cluster_low_16,get_first_sector_of_cluster(fat,entry->first_cluster_low_16)*512);
				uint16_t next_cluster=entry->first_cluster_low_16;
				while(1){
					next_cluster=get_fat_next_cluster(fat,first_fat_table,next_cluster);
					printf("%u ",next_cluster);
					if (next_cluster==4095){
						break;
					}
				}
				//print(to_string(get_fat_next_cluster(fat,first_fat_table,entry->first_cluster_low_16)));
				if (strstr(tmp_name,"TEST")){
					char* file=malloc(512);
					atapio_read_sectors(get_first_sector_of_cluster(fat,entry->first_cluster_low_16), 1, file);
					print(file);

				}
			}
			
			printchar('\n');
		}


	}

	printf("%sknknknknk","helloytfoucv");
	

	uint32_t x,y;

	while(1){
		get_cursor_pos(&x, &y);
		move_cursor(20, 0);
		for (int i = 0; i < 10; ++i){
			deletechar();
		}

		uint64_t time=(uint64_t)(TimeSinceBoot*100);
		print(to_string(time));
		printchar(' ');
		print(to_string(x));
		printchar(' ');
		print(to_string(y));

		move_cursor(x, y);
		sleep(50);
	}
	loop();


	return 123;
}
