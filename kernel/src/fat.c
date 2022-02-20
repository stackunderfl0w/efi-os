#include "fat.h"
#include "pit.h"


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
uint64_t get_first_free_cluster(fat_BS* part,uint8_t* fat_table){
	uint64_t cluster=0;
	while(get_fat_next_cluster(part,fat_table,cluster)){
		cluster++;
	}
	return cluster;
}
void set_fat_next_cluster(fat_BS* part,uint8_t* fat_table,uint64_t start_cluster,uint64_t next_cluster){
	uint64_t fat_offset = start_cluster *1.5;
	//uint64_t fat_sector = get_first_fat_sector(part) + (fat_offset / part->bytes_per_sector);
	uint64_t ent_offset = fat_offset % 512;
	if(start_cluster%2){
		(*(uint16_t*)&fat_table[ent_offset])|=next_cluster<<4;
	}
	else{
		(*(uint16_t*)&fat_table[ent_offset])|=0xFFF&next_cluster;
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
}
uint8_t* boot_sector;
uint8_t fs_type;
fat_BS* BS;
uint8_t* FAT_TABLE_0;
uint8_t* FAT_TABLE_1;
uint8_t* root_directory;


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
			//printf(" Cluster:%u at:%x ",entry->first_cluster_low_16,get_first_sector_of_cluster(BS,entry->first_cluster_low_16)*512);
			//printf("Size:%u\n",entry->size);
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
				for(int i=0; i<11;i++){

					if(entry->name[i]!=' '){
						if(entry->name[i]>='A'&&entry->name[i]<='Z'){
							tmp_name[index++]=entry->name[i]+32;
						}
						else{
							tmp_name[index++]=entry->name[i];
						}
					}
					if(i==7){
						if(entry->name[8]!=' '){
							tmp_name[index++]='.';
						}
					}	
				}
			}
			print(tmp_name);
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

FAT_DIRECTORY_ENTRY* create_entry_in_directory(fat_BS* part, char* start_entry, char* name){
	FAT_DIRECTORY_ENTRY* entry;
	uint64_t lfn_count=0;
	for (uint64_t i = 0; i<512; i+=32){
		printf("%x ",i);
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
		else if(start_entry[i]){
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
				int idex=0;
				for(int i=0; i<11;i++){

					if(entry->name[i]!=' '){
						if(entry->name[i]>='A'&&entry->name[i]<='Z'){
							tmp_name[index++]=entry->name[i]+32;
						}
						else{
							tmp_name[index++]=entry->name[i];
						}
					}
					if(i==7){
						if(entry->name[8]!=' '){
							tmp_name[index++]='.';
						}
					}	
				}
			}
			lfn_count=0;
			if (!strcmp(tmp_name,name)){
				//print(tmp_name);
				return entry;
			}
		}
		else{//blank entry
			//int len=strlen(name);
			memset(entry,0,32);
			//memcpy(entry,name,11);
			//entry->first_cluster_low_16=get_first_free_cluster((fat_BS*)boot_sector,FAT_TABLE_0);
			return entry;
		}
	}
	return entry;
}

uint64_t get_cluster_chain_length(uint16_t next_cluster){
	int c_index=0;
	while (1){		
		next_cluster=get_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0,next_cluster);
		c_index++;
		if (next_cluster==4095){
			break;
		}
	}
	return c_index;
}
uint8_t* load_fat_cluster_chain(uint16_t next_cluster){
	uint8_t* file =malloc(512*get_cluster_chain_length(next_cluster));
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
//potential speedup to look for multiple sequential sectors to read with one command instead of sector by sector.
uint8_t* read_file(char* filepath){
	uint8_t* cur_dir=malloc(512);

	int sections=0;
	char** paths=split_string_by_char(filepath,'/',&sections);
	
	//printf("sections: %u \n",sections);

	//for (int i = 0; i < sections; ++i){
		//printf("load %s\n", paths[i]);
	//}
	FAT_DIRECTORY_ENTRY* entry=get_entry_from_directory((fat_BS*)boot_sector,root_directory,paths[0]);
	for (int i = 1; i < sections; ++i){
		if(strcmp(paths[i],"")){
			atapio_read_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry->first_cluster_low_16), 1, cur_dir);
			//printf("trying to load %s\n", paths[i]);
			entry=get_entry_from_directory((fat_BS*)boot_sector,cur_dir,paths[i]);
		}
	}
	uint16_t next_cluster=entry->first_cluster_low_16;

	free(cur_dir);
	free(paths);
	return load_fat_cluster_chain(next_cluster);
}


void write_file(char* filepath, uint8_t* data, uint64_t size){
	uint8_t* cur_dir=malloc(512);
	//memcpy(cur_dir,root_directory,512);
	int sections=0;
	char** paths=split_string_by_char(filepath,'/',&sections);
	
	FAT_DIRECTORY_ENTRY* entry=get_entry_from_directory((fat_BS*)boot_sector,root_directory,paths[0]);
	uint64_t entry_location;//=get_first_root_dir_sector((fat_BS*)boot_sector);
	for (int i = 1; i < sections; ++i){
		if(strcmp(paths[i],"")){
			entry_location=entry->first_cluster_low_16;
			atapio_read_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry->first_cluster_low_16), 1, cur_dir);
			printf("trying to load %s\n", paths[i]);
			if(i<sections-1){
				entry=get_entry_from_directory((fat_BS*)boot_sector,cur_dir,paths[i]);
			}
		}
	}
	printf("blank entry slot found\n");
	//create new entry in our copy of the directory
	entry=create_entry_in_directory((fat_BS*)boot_sector,cur_dir,paths[sections-1]);
	memcpy(entry,paths[sections-1],11);
			
	printf("entry created\n");
	//find a free cluster on the disk, update the fat table and write the location to the entry


	uint64_t cluster=get_first_free_cluster((fat_BS*)boot_sector,FAT_TABLE_0);
	entry->first_cluster_low_16=cluster;
	entry->size=size;
	uint64_t next_cluster=0;
	uint64_t sectors_written=0;
	if(size<=512){
		printf("wrtietodisk\n");
		set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, cluster,4095);
		atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry->first_cluster_low_16), 1, data);
	}
	else{
		while(size>512){			
			printf("wrtietodisk2\n");
			next_cluster=get_first_free_cluster((fat_BS*)boot_sector,FAT_TABLE_0);
			set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, cluster,next_cluster);
			atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry->first_cluster_low_16), 1, data+(512+sectors_written++));
			cluster=next_cluster;
			size-=512;
		}
		set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, next_cluster,4095);
		atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry->first_cluster_low_16), 1, data+(512+sectors_written++));
	}
	set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, cluster,next_cluster);

	printf("cluster found\n");
	set_fat_next_cluster((fat_BS*)boot_sector,FAT_TABLE_0, cluster,4095);
	
	printf("cluster info updated\n");



	//rewrite directory to include new entey
	atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry_location), 1, cur_dir);
	printf("directory rewriten:%u\n",get_first_sector_of_cluster((fat_BS*)boot_sector,entry_location));

	//overwrite disk's fat table and its duplicate just in case
	atapio_write_sectors(get_first_fat_sector((fat_BS*)boot_sector),((fat_BS*)boot_sector)->table_size_16,FAT_TABLE_0);
	atapio_write_sectors(get_first_fat_sector((fat_BS*)boot_sector)+9,((fat_BS*)boot_sector)->table_size_16,FAT_TABLE_0);
	printf("fat tables rewriten\n");

	//write data to disk
	//atapio_write_sectors(get_first_sector_of_cluster((fat_BS*)boot_sector,entry->first_cluster_low_16), (size+511)/512, data);
	printf("date writen\n");
	free(paths);
	sleep(5000);
}

uint64_t get_filesize(char* filepath){

	//uint8_t* file =malloc((entry->size&0xffffff00)+512);
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
	for (int i = 0; i < 256; ++i){
		auto tmp=names[0][i];
		printf("%c ",tmp);
		//printchar(' ');
	}

*/

