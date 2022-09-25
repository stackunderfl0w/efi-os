//
// Created by pat on 9/11/22.
//

#include "file_table.h"
#include "fat.h"
#include "memory.h"
int file_entry_cmp(file_table_entry* first, file_table_entry* second){
	return first->fd-second->fd;
}
file_table* init_file_table(){
	file_table* ft=malloc(sizeof(file_table));
	ft->entries=create_sorted_list((int (*)(void *, void *)) file_entry_cmp);
	ft->next_id=0;
	return ft;
}
uint64_t file_read(file_table* ft,int fd, uint8_t* buf, uint64_t len){
	file_table_entry tmp={fd,0};
	file_table_entry* file = sorted_list_get(ft->entries, (void *)&tmp);
	memcpy(buf,file->base->seek_head,len);
	file->base->seek_head+=len;
	return len;
}

int file_fstat(file_table* ft,int fd, struct stat *statbuf) {
	file_table_entry tmp={fd,0};
	file_table_entry* file = sorted_list_get(ft->entries, (void *)&tmp);
	statbuf->st_ino=(uint64_t)file->base;
	statbuf->st_size=file->base->size;
	return 0;
}
int create_file_table_entry(file_table* ft, vfs_node* file){
	file_table_entry* fe=malloc(sizeof(file_table_entry));
	fe->fd=ft->next_id++;
	fe->base=file;
	sorted_list_insert(ft->entries,fe);
	return fe->fd;
}
int remove_file_table_entry(file_table* ft,int fd){
	file_table_entry tmp={fd,0};
	file_table_entry* file = sorted_list_get(ft->entries, (void *)&tmp);
	vfs_close_file(file->base);
	sorted_list_remove(ft->entries,(void *)&tmp);
	free(file);
}




