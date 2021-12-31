#pragma once
#include "typedef.h"
#include "string.h"
	typedef struct{
		char magic[3];
		char oem[8];
		uint16_t 	bytes_per_sector;
		uint8_t 	sectors_per_cluster;
		uint16_t 	reserved_sectors;
		uint8_t 	fat_table_count;
		uint16_t 	directory_entries;
		uint16_t 	sectors;
		uint8_t 	media_descriptor_type;
		uint16_t 	sectors_per_fat;
		uint16_t	sectors_per_track;
		uint16_t	head_count;
		uint32_t	hidden_sectors;
		uint32_t	large_sector_count;
	}__attribute__((packed))FAT_BPB;
typedef struct {
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short 	    bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
 
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	unsigned char		extended_section[54];
 
}__attribute__((packed)) fat_BS;


typedef struct{
	char name[11];
	uint8_t attributes;
	uint8_t reserved_nt;
	uint8_t creation_durration;
	uint16_t creation_time;
	uint16_t creation_date;
	uint16_t last_accessed_date;
	uint16_t first_cluster_high_16;
	uint16_t last_modified_time;
	uint16_t last_modified_date;
	uint16_t first_cluster_low_16;
	uint32_t size;
}__attribute__((packed))FAT_DIRECTORY_ENTRY;

typedef struct{
	uint8_t order;
	uint16_t first_5[5];
	uint8_t attribute;
	uint8_t type;
	uint8_t checksum;
	uint16_t next_6[6];
	uint16_t zero;
	uint16_t last_2[2];
}__attribute__((packed))FAT_LONG_NAME_ENTRY;

typedef struct{
	char name[256];
	uint64_t location;
	uint64_t size;
	bool directory;
	char flags;

}FILE_ENTRY;


#define FAT12 1
#define FAT16 2
#define FAT32 3
#define ExFAT 4


uint64_t get_total_sectors(fat_BS* part){
	uint64_t total_sectors = (part->total_sectors_16 == 0)? part->total_sectors_32 : part->total_sectors_16;
	return total_sectors;
}
uint64_t get_fat_size(fat_BS* part){
	uint64_t fat_size = part->table_size_16;
	return fat_size;
}

uint64_t get_root_dir_sectors(fat_BS* part){
	uint64_t root_dir_sectors = ((part->root_entry_count * 32) + (part->bytes_per_sector - 1)) / part->bytes_per_sector;
	return root_dir_sectors;
}

uint64_t get_first_data_sector(fat_BS* part){
	uint64_t first_data_sector = part->reserved_sector_count + (part->table_count * get_fat_size(part)) + get_root_dir_sectors(part);	
	return first_data_sector;
}

uint64_t get_first_fat_sector(fat_BS* part){
	uint64_t first_fat_sector = part->reserved_sector_count;
	return first_fat_sector;
}

uint64_t get_data_sectors(fat_BS* part){
	uint64_t data_sectors = get_total_sectors(part) - (part->reserved_sector_count + (part->table_count * get_fat_size(part)) + get_root_dir_sectors(part));
	return data_sectors;
}

uint64_t get_total_clusters(fat_BS* part){
	uint64_t total_clusters = get_data_sectors(part) / part->sectors_per_cluster;
	return total_clusters;
}
uint64_t get_first_root_dir_sector(fat_BS* part){
	uint64_t first_root_dir_sector = get_first_data_sector(part) - get_root_dir_sectors(part);
	return first_root_dir_sector;
}
uint64_t get_first_sector_of_cluster(fat_BS* part,uint64_t cluster){
	uint64_t first_sector_of_cluster = ((cluster - 2) * part->sectors_per_cluster) + get_first_data_sector(part);
	return first_sector_of_cluster;
}

uint64_t get_fat_next_cluster(fat_BS* part,uint8_t* fat_table, uint64_t cluster){
	uint64_t fat_offset = cluster *1.5;
	//uint64_t fat_sector = get_first_fat_sector(part) + (fat_offset / part->bytes_per_sector);
	uint64_t ent_offset = fat_offset % 512;
	if(cluster%2){
		return (*(uint16_t*)&fat_table[ent_offset])>>4;
	}
	return (*(uint16_t*)&fat_table[ent_offset])&0xFFF;
}
int ident_fat(fat_BS* part){
	int fat_type;
	if (part->bytes_per_sector == 0) {
	   fat_type = ExFAT;
	}
	else if(get_total_clusters(part) < 4085) {
	   fat_type = FAT12;
	} 
	else if(get_total_clusters(part) < 65525) {
	   fat_type = FAT16;
	} 
	else{
	   fat_type = FAT32;
	}
}
uint8_t* boot_sector;
uint8_t fs_type;
fat_BS* BS;
uint8_t* FAT_TABLE_0;
uint8_t* FAT_TABLE_1;
uint8_t* root_directory;


struct FAT_PARTITION{

};
//will later diferentiate fs types but hardcoded to fat12 for now
void INIT_FILESYSTEM(){
	boot_sector=malloc(512);
	atapio_read_sectors(0, 1, boot_sector);

	fs_type=ident_fat((fat_BS*)boot_sector);
	BS=(fat_BS*) boot_sector;

	FAT_TABLE_0=malloc(512*BS->table_size_16);
	atapio_read_sectors(get_first_fat_sector(BS),BS->table_size_16,FAT_TABLE_0);

	root_directory=malloc(512);
	atapio_read_sectors(get_first_root_dir_sector(BS), 1, root_directory);



}
char tmp_long[256];
char tmp_name[256];
char** read_directory(char* filepath,int *entries){
	char* temp_dir=malloc(2048);
	int lfn_count=0;
	for (uint64_t i = 0; root_directory[i]; i+=32){
		if(root_directory[i]==0xE5){
			continue;
		}
		if(root_directory[i+11]==0xf){
			FAT_LONG_NAME_ENTRY* lfn=(FAT_LONG_NAME_ENTRY*)&root_directory[i];
			//print("lfn: ");
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
			tmp_long[14+(16*lfn_count)]=0;
			lfn_count++;
		}
		else{
			FAT_DIRECTORY_ENTRY* entry=(FAT_DIRECTORY_ENTRY*)&root_directory[i];
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
			printf(" Cluster:%u at:%x ",entry->first_cluster_low_16,get_first_sector_of_cluster(BS,entry->first_cluster_low_16)*512);
			printf("Size:%u\n",entry->size);
			if(entry->attributes&0x10){
				//print("directory\n");
			}
			uint16_t next_cluster=entry->first_cluster_low_16;
			lfn_count=0;
		}
	}

	return 0;
}
FAT_DIRECTORY_ENTRY* get_entry_from_directory(fat_BS* part, char* start_entry,char* name){
	FAT_DIRECTORY_ENTRY* entry;
	uint64_t lfn_count=0;
	for (uint64_t i = 0; start_entry[i]; i+=32){
		if(start_entry[i]==0xE5){
			continue;
		}
		if(start_entry[i+11]==0xf){
			FAT_LONG_NAME_ENTRY* lfn=(FAT_LONG_NAME_ENTRY*)&start_entry[i];
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
			lfn_count++;
		}
		else{
			entry=(FAT_DIRECTORY_ENTRY*)&start_entry[i];
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
			//print(tmp_name);
			//printf("Cluster:%u at:%x\n",entry->first_cluster_low_16,get_first_sector_of_cluster(part,entry->first_cluster_low_16)*512);
			//printf("Size:%u\n",entry->size);
			//if(entry->attributes&0x10){
			//	print("directory\n");
			//}
			uint16_t next_cluster=entry->first_cluster_low_16;
			lfn_count=0;
			if (!strcmp(tmp_name,name)){
				print("FOUNDIT");
				print(tmp_name);

				return entry;
			}

		}
	}
	return entry;
}
uint8_t* read_file(char* filepath){
	uint8_t* cur_dir=malloc(512);
	memcpy(cur_dir,root_directory,512);
	int sections=0;
	char** paths=split_string_by_char(filepath,'/',&sections);
	
	printf("sections: %u \n",sections);

	for (int i = 0; i < sections; ++i){
		printf("load %s\n", paths[i]);
	}
	FAT_DIRECTORY_ENTRY* entry=get_entry_from_directory((fat_BS*)boot_sector,root_directory,paths[0]);
	for (int i = 1; i < sections; ++i){
		if(strcmp(paths[i],"")){
			atapio_read_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry->first_cluster_low_16), 1, cur_dir);
			printf("trying to load %s\n", paths[i]);
			entry=get_entry_from_directory((fat_BS*)boot_sector,root_directory,paths[i]);
		}
	}
	uint8_t* file =malloc((entry->size&0xffffff00)+512);
	uint16_t next_cluster=entry->first_cluster_low_16;
	int c_index=0;

	while (1){
		atapio_read_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,next_cluster), 1, file+(512*c_index));
		
		next_cluster=get_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0,next_cluster);
		c_index++;
		if (next_cluster==4095){
			break;
		}
	}
	return file;

}
uint64_t get_filesize(char* filepath){

}
/*
char* file=(char*)"/EFI/BOOT/BOOTX64.efi";
	char names[16][16];
	memset(names,0,256);
	int index=0;
	int n_index=0;
	int w_index=0;
	while(file[index]){
		names[n_index][w_index++]=file[index++];
		if(file[index]=='/'){
			names[n_index][w_index]=0;
			n_index++;
			w_index=0;
		}
	}
	for(unsigned int i =0; i<=n_index;i++){
		printf("%u %s\n",i,names[0]+16*i);
	}
	for (int i = 0; i < 256; ++i)
					{
						auto tmp=names[0][i];
						printf("%c ",tmp);
						//printchar(' ');
					}

*/

