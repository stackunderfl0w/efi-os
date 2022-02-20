#pragma once
#include "typedef.h"
#include "string.h"
#include "ata-pio.h"
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


uint64_t get_total_sectors(fat_BS* part);
uint64_t get_fat_size(fat_BS* part);

uint64_t get_root_dir_sectors(fat_BS* part);

uint64_t get_first_data_sector(fat_BS* part);
uint64_t get_first_fat_sector(fat_BS* part);
uint64_t get_data_sectors(fat_BS* part);

uint64_t get_total_clusters(fat_BS* part);
uint64_t get_first_root_dir_sector(fat_BS* part);
uint64_t get_first_sector_of_cluster(fat_BS* part,uint64_t cluster);

uint64_t get_fat_next_cluster(fat_BS* part,uint8_t* fat_table, uint64_t cluster);
uint64_t get_first_free_cluster(fat_BS* part,uint8_t* fat_table);
void set_fat_next_cluster(fat_BS* part,uint8_t* fat_table,uint64_t start_cluster,uint64_t next_cluster);
int ident_fat(fat_BS* part);


struct FAT_PARTITION{

};
//will later diferentiate fs types but hardcoded to fat12 for now
void INIT_FILESYSTEM();

char** read_directory(char* filepath,int *entries);

FAT_DIRECTORY_ENTRY* get_entry_from_directory(fat_BS* part, char* start_entry,char* name);

FAT_DIRECTORY_ENTRY* create_entry_in_directory(fat_BS* part, char* start_entry, char* name);


uint8_t* read_file(char* filepath);

void write_file(char* filepath, uint8_t* data, uint64_t size);

uint64_t get_filesize(char* filepath);