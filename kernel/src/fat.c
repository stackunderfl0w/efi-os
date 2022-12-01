#include "fat.h"
#include "ata-pio.h"
#include "ctype.h"
#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"

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
	uint64_t fat_offset = cluster*3/2;
	//uint64_t fat_sector = get_first_fat_sector(part) + (fat_offset / part->bytes_per_sector);
	//uint64_t ent_offset = fat_offset % 512;
	if(cluster%2){
		return (*(uint16_t*)&fat_table[fat_offset])>>4;
	}
	return (*(uint16_t*)&fat_table[fat_offset])&0xFFF;
}
uint64_t get_first_free_cluster(fat_BS* part,uint8_t* fat_table){
	uint64_t cluster=0;
	while(get_fat_next_cluster(part,fat_table,cluster)){
		cluster++;
	}
	//set the next cluster ahead of time to prevent the same cluster being found multiple times
	set_fat_next_cluster(part, fat_table, cluster, 4095);
	return cluster;
}
void set_fat_next_cluster(fat_BS* part,uint8_t* fat_table,uint64_t start_cluster,uint64_t next_cluster){
	uint64_t fat_offset = start_cluster *3/2;
	//uint64_t fat_sector = get_first_fat_sector(part) + (fat_offset / part->bytes_per_sector);
	//uint64_t ent_offset = fat_offset % 512;
	if(start_cluster%2){
		(*(uint16_t*)&fat_table[fat_offset])=((*(uint16_t*)&fat_table[fat_offset])&0x000F)|(next_cluster<<4);
	}
	else{
		(*(uint16_t*)&fat_table[fat_offset])=((*(uint16_t*)&fat_table[fat_offset])&0xF000)|(0xFFF&next_cluster);
	}
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
	return fat_type;
}
uint8_t* boot_sector;
uint8_t fs_type;
fat_BS* BS;
uint8_t* FAT_TABLE_0;
uint8_t* FAT_TABLE_1;
uint8_t* root_directory;


//will later diferentiate fs types but hardcoded to fat12 for now
void INIT_FILESYSTEM(){
	boot_sector=kmalloc(512);
	atapio_read_sectors(0, 1, boot_sector);
	fs_type=ident_fat((fat_BS*)boot_sector);
	BS=(fat_BS*) boot_sector;

	FAT_TABLE_0=kmalloc(512*BS->table_size_16);
	atapio_read_sectors(get_first_fat_sector(BS),(uint8_t)BS->table_size_16,FAT_TABLE_0);

	root_directory=kmalloc(512*get_root_dir_sectors(BS));
	atapio_read_sectors(get_first_root_dir_sector(BS), get_root_dir_sectors(BS), root_directory);
}
void read_lfn_entry_name(char* buf, FAT_LONG_NAME_ENTRY* lfn){
	for (int i = 0; i < 5; ++i){
		buf[i]=(char)lfn->first_5[i];
	}
	for (int i = 0; i < 6; ++i){
		buf[i+5]=(char)lfn->next_6[i];
	}
	for (int i = 0; i < 2; ++i){
		buf[i+11]=(char)lfn->last_2[i];
	}
}
void read_f12_entry_name(char* buf, FAT_DIRECTORY_ENTRY* entry){
	int index=0;
	for(int i=0; i<11;i++){
		if(entry->name[i]!=' ')
			buf[index++]=entry->name[i];
		//add a dot if there is a file extension
		if(i==7&&isalpha(entry->name[8]))
			buf[index++]='.';
	}
	buf[index++]=0;
}
FAT_DIRECTORY_ENTRY* get_next_fat_entry(FAT_DIRECTORY_ENTRY* entry, char* buf){

}
FAT_DIRECTORY_ENTRY* get_entry_from_directory(fat_BS* part, unsigned char* start_entry,char* name){
	char tmp_name[256];
	FAT_DIRECTORY_ENTRY* entry;
	bool lfn_found=false;
	for (uint64_t i = 0; start_entry[i]; i+=32){
		if(start_entry[i]==0xE5){
			continue;
		}
		if(start_entry[i+11]==0xf){
			FAT_LONG_NAME_ENTRY* lfn=(FAT_LONG_NAME_ENTRY*)&start_entry[i];
			if(!lfn_found){
				memset(tmp_name,0,256);
				lfn_found=true;
			}
			int loc=(lfn->order&0x1f)-1;
			read_lfn_entry_name(tmp_name+(13*loc),lfn);
		}
		else{
			entry=(FAT_DIRECTORY_ENTRY*)&start_entry[i];
			if(!lfn_found){
				read_f12_entry_name(tmp_name,entry);
			}
			lfn_found=false;
			if (!strcasecmp(tmp_name,name)){
				return entry;
			}
		}
	}
	return entry;
}

FAT_DIRECTORY_ENTRY* create_entry_in_directory(fat_BS* part, uint8_t* start_entry, char* name){
	char tmp_long[256],tmp_name[256];
	FAT_DIRECTORY_ENTRY* entry;
	bool lfn_found=false;
	for (uint64_t i = 0; i<512; i+=32){
		if(start_entry[i]==0xE5){
			continue;
		}
		if(start_entry[i+11]==0xf){
			FAT_LONG_NAME_ENTRY* lfn=(FAT_LONG_NAME_ENTRY*)&start_entry[i];
			if(!lfn_found){
				memset(tmp_long,0,256);
				lfn_found=true;
			}
			int loc=(lfn->order&0x1f)-1;
			read_lfn_entry_name(tmp_long+(13*loc),lfn);
		}
		else if(start_entry[i]){
			entry=(FAT_DIRECTORY_ENTRY*)&start_entry[i];
			if(!lfn_found){
				read_f12_entry_name(tmp_name,entry);
			}
			lfn_found=false;
			if (!strcasecmp(tmp_name,name)){
				return entry;
			}
		}
		else{//blank entry
			memset(entry,0,32);
			return entry;
		}
	}
	return entry;
}

uint64_t get_cluster_chain_length(uint16_t next_cluster){
	//check for root dir
	if(next_cluster==0)
		return(get_root_dir_sectors(BS));

	int c_index=0;
	while (next_cluster!=4095){
		next_cluster=get_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0,next_cluster);
		c_index++;
	}
	return c_index;
}
uint8_t* load_fat_cluster_chain(uint16_t next_cluster){
	//check if we are trying to access the root directory
	if(next_cluster==0){
		uint8_t* file=kmalloc(512*get_root_dir_sectors(BS));
		memcpy(file,root_directory,512*get_root_dir_sectors(BS));
		return file;
	}
	uint8_t* file =kmalloc(512*get_cluster_chain_length(next_cluster));
	int c_index=0;
	while (next_cluster!=4095){
		atapio_read_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,next_cluster), 1, file+(512*c_index));
		next_cluster=get_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0,next_cluster);
		c_index++;
	}
	return file;
}
void write_fat_cluster_chain(uint16_t cluster, uint8_t* data, uint64_t size){
	//check if we are trying to access the root directory
	if(cluster==0){
		memcpy(root_directory,data,512*get_root_dir_sectors(BS));
	}
	uint64_t len= get_cluster_chain_length(cluster);
	uint64_t next_cluster=0;
	uint64_t sectors_written=0;
	if(size<=512){
		set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, cluster,4095);
		atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,cluster), 1, data);
	}
	else{
		while(size>512){
			next_cluster=get_first_free_cluster((fat_BS*)boot_sector,FAT_TABLE_0);
			set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, next_cluster,4095);

			set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, cluster,next_cluster);

			atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,cluster), 1, data+(512*sectors_written));
			sectors_written++;
			cluster=next_cluster;
			size-=512;
		}
		atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,next_cluster), 1, data+(512*sectors_written));
		sectors_written++;
	}
}
uint64_t get_file_base_cluster(char* filepath){
	int sections=0;
	char** paths=split_string_by_char(filepath,'/',&sections);

	FAT_DIRECTORY_ENTRY* entry=get_entry_from_directory((fat_BS*)boot_sector,root_directory,paths[0]);
	uint16_t cluster=entry->first_cluster_low_16;
	for (int i = 1; i < sections; ++i){
		if(strcasecmp(paths[i],"")){
			//load next directory
			uint8_t* cur_dir=load_fat_cluster_chain(cluster);
			//get location of next directory
			entry=get_entry_from_directory((fat_BS*)boot_sector,cur_dir,paths[i]);
			//save location and free unneeded directory
			cluster=entry->first_cluster_low_16;
			free(cur_dir);
		}
	}
	free(paths);
	return cluster;
}
//potential speedup to look for multiple sequential sectors to read with one command instead of sector by sector.
uint8_t* read_file(char* filepath){
	return load_fat_cluster_chain(get_file_base_cluster(filepath));
}

void read_cluster_chain_from_offset(uint64_t cluster, uint8_t* buf, uint64_t base, uint64_t sectors){
	for (int i = 0; i < base; ++i) {
		cluster=get_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0,cluster);
	}
	int c_index=0;
	for (int i = 0; i < sectors&&cluster!=4095; ++i) {
		atapio_read_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,cluster), 1, buf+(512*c_index));
		cluster=get_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0,cluster);
		c_index++;
	}
}
///todo add support for overwriting file
void write_file(char* filepath, uint8_t* data, uint64_t size){
	int sections=0;
	char** paths=split_string_by_char(filepath,'/',&sections);

	FAT_DIRECTORY_ENTRY* entry=get_entry_from_directory((fat_BS*)boot_sector,root_directory,paths[0]);
	uint64_t entry_location;//=get_first_root_dir_sector((fat_BS*)boot_sector);
	uint8_t* cur_dir;
	for (int i = 1; i < sections; ++i){
		if(strcasecmp(paths[i],"")){
			entry_location=entry->first_cluster_low_16;
			cur_dir=load_fat_cluster_chain(entry_location);
			//kprintf("trying to load %s\n", paths[i]);
			if(i<sections-1){
				free(cur_dir);
			}
			entry=get_entry_from_directory((fat_BS*)boot_sector,cur_dir,paths[i]);
		}
	}
	//create new entry in our copy of the directory
	entry=create_entry_in_directory((fat_BS*)boot_sector,cur_dir,paths[sections-1]);
	memcpy(entry,paths[sections-1],11);

	//find a free cluster on the disk, update the fat table and write the location to the entry


	uint64_t cluster=get_first_free_cluster((fat_BS*)boot_sector,FAT_TABLE_0);

	entry->first_cluster_low_16=cluster;
	entry->size=size;
	write_fat_cluster_chain(cluster,data,size);


	//rewrite directory to include new entey
	///todo add support for increasing size of directory
	atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry_location), get_cluster_chain_length(entry_location), cur_dir);

	//overwrite disk's fat table and its duplicate just in case
	atapio_write_sectors(get_first_fat_sector((fat_BS*)boot_sector),((fat_BS*)boot_sector)->table_size_16,FAT_TABLE_0);
	atapio_write_sectors(get_first_fat_sector((fat_BS*)boot_sector)+9,((fat_BS*)boot_sector)->table_size_16,FAT_TABLE_0);

	free(paths);
	free(cur_dir);
}

void de_INIT_FILESYSTEM() {
	free(boot_sector);
	free(FAT_TABLE_0);
	free(root_directory);
}

void fat_populate_vfs_directory(vfs_node* dir, char* dir_path){
	//have base declared as root dir and call get base cluster if not due to fat 12 being weird and storing the root dir separately
	uint8_t *start_entry=root_directory;
	uint64_t dir_size=14;//size of root directory in clusters
	if(strcmp(dir_path,"/")!=0){
		uint16_t base_cluster=get_file_base_cluster(dir_path);
		start_entry=load_fat_cluster_chain(base_cluster);
		dir_size=get_cluster_chain_length(base_cluster);
	}

	char tmp_name[256];
	FAT_DIRECTORY_ENTRY* entry;
	bool lfn_found=false;
	for (uint64_t i = 0; start_entry[i]&&i<512*dir_size; i+=32){
		if(start_entry[i]==0xE5)
			continue;
		if(start_entry[i+11]==0xf){
			FAT_LONG_NAME_ENTRY* lfn=(FAT_LONG_NAME_ENTRY*)&start_entry[i];
			if(!lfn_found)
				memset(tmp_name,0,256);
			lfn_found=true;
			int loc=(lfn->order&0x1f)-1;
			read_lfn_entry_name(tmp_name+(13*loc),lfn);
		}
		else{
			entry=(FAT_DIRECTORY_ENTRY*)&start_entry[i];
			if(!lfn_found){
				read_f12_entry_name(tmp_name,entry);
			}
			lfn_found=false;
			if(strcmp(tmp_name,".")==0|strcmp(tmp_name,"..")==0){
				continue;
			}
			vfs_node* n=kmalloc(sizeof(vfs_node));
			strcpy(n->name,tmp_name);
			n->size=entry->size;
			n->drive_id=0;
			n->location=entry->first_cluster_low_16;
			n->flags=0;

			//if it's not a directory lets just say it's a file.
			n->flags|=entry->attributes&FAT12_DIRECTORY?VFS_DIRECTORY:VFS_FILE;
			if(n->flags&VFS_DIRECTORY){
				n->children= create_sorted_list(dir->children->cmp);
			}
			n->open_references=0;
			n->block_size=512;
			n->parent=dir;
			sorted_list_insert(dir->children,n);
		}
	}
	if(start_entry!=root_directory){
		free(start_entry);
	}
}


