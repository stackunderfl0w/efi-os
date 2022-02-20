#include "typedef.h"
#include "bitmap-font.h"
#include "graphics.h"
#include "gdt.h"
#include "idt.h"
#include "pit.h"
#include "efimemory.h"
#include "mouse.h"
#include "paging.h"
#include "memory.h"
#include "serial.h"
#include "ata-pio.h"
#include "fat.h"

#include "time.h"
#include "keyboard.h"
#include "shell.h"

typedef struct {
	Framebuffer* buf;
	bitmap_font* font;
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


	print("idt loaded ");

	//SET_PIT_DIVISOR(65535);
	SET_PIT_FREQUENCY(1000);


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
	sleep(1000);


	INIT_HEAP((void*)0x8000000000,0x10);



	uint8_t* sector_1=malloc(512);;

	atapio_software_reset(ATAPIO_REGULAR_STATUS_REGISTER_PORT);

	atapio_read_sectors(0, 1, sector_1);

	/*for (int i = 0; i < 512; ++i)
	{
		uint64_t tmp=sector_1[i];
		print(to_hstring_noformat(tmp));
		printchar(' ');
	}*/
	/*if (sector_1[0]==0xEB&&sector_1[1]==0x3C&&sector_1[2]==0x90){
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

		//loop();

		printf("Root dir %u %x\n",get_first_root_dir_sector(fat),512*get_first_root_dir_sector(fat));

		uint8_t* root_dir_entry=malloc(512);
		uint8_t* first_fat_table=malloc(512);

		atapio_read_sectors(get_first_root_dir_sector(fat), 1, root_dir_entry);
		atapio_read_sectors(get_first_fat_sector(fat),1,first_fat_table);
		char tmp_name[256];
		char tmp_long[256];
		for (uint64_t i = 0; root_dir_entry[i]; i+=32){
			if(root_dir_entry[i]==0xE5){
				continue;
			}
			if(root_dir_entry[i+11]==0xf){
				FAT_LONG_NAME_ENTRY* lfn=(FAT_LONG_NAME_ENTRY*)&root_dir_entry[i];
				print("lfn: ");
				memset(tmp_long,0,16);
				for (int i = 0; i < 5; ++i){
					tmp_long[i]=(char)lfn->first_5[i]&0xff;
				}
				for (int i = 0; i < 6; ++i){
					tmp_long[i+5]=(char)lfn->next_6[i]&0xff;
				}
				for (int i = 0; i < 2; ++i){
					tmp_long[i+11]=(char)lfn->last_2[i]&0xff;
				}
				tmp_long[14]=0;
				print(tmp_long);
			}
			else{
				FAT_DIRECTORY_ENTRY* entry=(FAT_DIRECTORY_ENTRY*)&root_dir_entry[i];
				memset(tmp_name,0,256);

				memcpy(tmp_name,entry->name,11);
				
				tmp_name[11]=0;
				print(tmp_name);
				printf("Cluster:%u at:%x\n",entry->first_cluster_low_16,get_first_sector_of_cluster(fat,entry->first_cluster_low_16)*512);
				printf("Size:%u\n",entry->size);
				if(entry->attributes&0x10){
					print("directory\n");
				}
				uint16_t next_cluster=entry->first_cluster_low_16;
				while(1){
					next_cluster=get_fat_next_cluster(fat,first_fat_table,next_cluster);
					printf("%u ",next_cluster);
					if (next_cluster==4095){
						break;
					}
				}
				if (strstr(tmp_name,"TEST")){
					printchar('U');
					uint8_t* file=malloc(512);
					atapio_read_sectors(get_first_sector_of_cluster(fat,entry->first_cluster_low_16), 1, file);
					print((char*)file);
					print(to_hstring(crc32b(file,entry->size)));
					free(file);

				}
				//print(to_string(get_fat_next_cluster(fat,first_fat_table,entry->first_cluster_low_16)));
				if (strstr(tmp_name,"STARTUP")){
					uint8_t* file=malloc((entry->size&0xffffff00)+512);
					next_cluster=entry->first_cluster_low_16;
					int c_index=0;

					while (1){
						atapio_read_sectors(get_first_sector_of_cluster(fat,next_cluster), 1, file+(512*c_index));
						
						next_cluster=get_fat_next_cluster(fat,first_fat_table,next_cluster);
						c_index++;
						if (next_cluster==4095){
							break;
						}
					}
					//print((char*)file);
					print(to_hstring(crc32b(file,entry->size)));
					free(file);
				}
				if (strstr(tmp_name,"RES")){
					print("\n\n\n");
					uint8_t* file=malloc(512);
					memset(file,0,512);
					next_cluster=entry->first_cluster_low_16;
					printf("ld %u",entry->first_cluster_low_16);
					int lfn_count=0;
					atapio_read_sectors(get_first_sector_of_cluster(fat,entry->first_cluster_low_16), 1, file);
					for (uint64_t i = 0; file[i]; i+=32){
						if(file[i]==0xE5){
							continue;
						}
						if(file[i+11]==0xf){
							FAT_LONG_NAME_ENTRY* lfn=(FAT_LONG_NAME_ENTRY*)&file[i];
							print("lfn: ");
							if(lfn_count==0){
								memset(tmp_long,0,256);
							}
							for (int i = 0; i < 5; ++i){
								tmp_long[i+(16*lfn_count)]=(char)lfn->first_5[i]&0xff;
							}
							for (int i = 0; i < 6; ++i){
								tmp_long[i+5+(16*lfn_count)]=(char)lfn->next_6[i]&0xff;
							}
							for (int i = 0; i < 2; ++i){
								tmp_long[i+11+(16*lfn_count)]=(char)lfn->last_2[i]&0xff;
							}
							tmp_long[14]=0;
							print(tmp_long+16*lfn_count);
							printchar('\n');
							lfn_count++;
						}
						else{
							FAT_DIRECTORY_ENTRY* entry=(FAT_DIRECTORY_ENTRY*)&file[i];
							memset(tmp_name,0,256);
							int index=0;
							if(lfn_count){
								for(int i=lfn_count-1;i>=0;i--){
									int long_index=0;
									while(tmp_long[16*i+long_index]){
										tmp_name[index++]=tmp_long[16*i+long_index++];
									}
									
								}
								tmp_name[index]=0;
							}
							else{
								memcpy(tmp_name,entry->name,11);
								tmp_name[11]=0;
							}
							print(tmp_name);
							printf("Cluster:%u at:%x\n",entry->first_cluster_low_16,get_first_sector_of_cluster(fat,entry->first_cluster_low_16)*512);
							printf("Size:%u\n",entry->size);
							if(entry->attributes&0x10){
								print("directory\n");
							}
							uint16_t next_cluster=entry->first_cluster_low_16;
							lfn_count=0;
						}
						//sleep(500);
					}
					//for (int i = 0; i < 512; ++i)
					//{
					//	uint64_t tmp=file[i];
					//	print(to_hstring_noformat(tmp));
					//	printchar(' ');
					//}


					free(file);
				}
			}
			printchar('\n');
		}
	}*/

	printf("%sknknknknk\n","helloytfoucv");

	INIT_FILESYSTEM();
	
	int file_entries;
	//read_directory("/",&file_entries);
	
	//uint8_t* file = read_file("/resources/TEST    TXT");
	uint8_t* file = read_file("/resources/config.txt");
	//uint8_t* file = read_file("/resources/startup.txt");

	uint8_t* font2 = read_file("/resources/zap-light16.psf");

	bitmap_font loaded_font=load_font(font2);

	init_text_overlay(info->buf, &loaded_font);

	write_file("/resources/WRTTEST TXT",file,1024);

	//file = read_file("/resources/WRTTEST TXT");

	//printf("\n%s\n",file);
	print(file);
	uint32_t x,y;


	char* chr_ptr=malloc(13);
	char* src="Hello there";
	memcpy(chr_ptr,src,12);
	printf(chr_ptr);
	realloc(chr_ptr,256);
	printf(chr_ptr);

	//printf(chr_ptr);

	INIT_RTC();

	run_shell(info->buf, info->font);



	while(1){
		//printf("second:%u minute:%u hour:%u day:%u month:%u year:%u\n",
			//SYSTEM_TIME.second,SYSTEM_TIME.minute,SYSTEM_TIME.hour,SYSTEM_TIME.day,SYSTEM_TIME.month,SYSTEM_TIME.year);
		//printf("Day of week: %s\n",days_of_the_week[dayofweek()]);
		//printf("second:%u ",
		//SYSTEM_TIME.second);
		get_cursor_pos(&x, &y);

		move_cursor(40, 0);
		for(int i=0; i<40;i++){
			deletechar();
		}
		printf("%s %u %s, %u:%u:%u", days_of_the_week[dayofweek()], SYSTEM_TIME.day, months_short[SYSTEM_TIME.month], SYSTEM_TIME.hour, SYSTEM_TIME.minute, SYSTEM_TIME.second );
		move_cursor(x, y);

		sleep(100);
	}



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
