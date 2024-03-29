#include "file_table.h"
#include "memory.h"
int file_entry_cmp(file_table_entry* first, file_table_entry* second){
	return first->fd-second->fd;
}
int file_entry_search_cmp(file_table_entry* first, uint64_t second){
	return first->fd-second;
}
file_table* init_file_table(){
	file_table* ft=kmalloc(sizeof(file_table));
	ft->entries=create_sorted_list((int (*)(void *, void *))file_entry_cmp,(int (*)(void *, void *))file_entry_search_cmp);
	ft->next_id=0;
	return ft;
}
int64_t file_read(file_table* ft, int fd, uint8_t* buf, uint64_t len){
	file_table_entry* file = sorted_list_get(ft->entries, (void *)(uint64_t)fd);
	///todo deal with future error codes
	int64_t read = vfs_read(file->base,buf,file->seek_offset,len);

    file->seek_offset+=read;
	if(read==0){
		file->seek_offset=file->base->size;
	}
	return read;
}

int64_t file_write(file_table* ft, int fd, const uint8_t* buf, uint64_t len){
	file_table_entry* file = sorted_list_get(ft->entries, (void *)(uint64_t)fd);
	///todo deal with future error codes
	int64_t written = vfs_write(file->base,buf,file->seek_offset,len);
	file->seek_offset+=written;
	return written;
}

int64_t file_seek(file_table* ft,int fd, off_t offset, int whence){
	file_table_entry* file = sorted_list_get(ft->entries, (void *)(uint64_t)fd);
	int64_t intended=file->seek_offset;
	switch (offset) {
		case SEEK_SET: intended= whence;break;
		case SEEK_CUR: intended+=whence;break;
		case SEEK_END: intended=(int64_t)file->base->size+whence;break;
		default:return -1;
	}
	///todo add errno once implemented
	if(intended<0)
		return -1;
	return file->seek_offset=intended;
}

int file_fstat(file_table* ft,int fd, struct stat *stat_buf) {
	file_table_entry* file = sorted_list_get(ft->entries, (void *)(uint64_t)fd);

	stat_buf->st_ino=(uint64_t)file->base;
	stat_buf->st_size=file->base->size;
	return 0;
}
int create_file_table_entry(file_table* ft, vfs_node* file){
	file_table_entry* fe=kmalloc(sizeof(file_table_entry));
	*fe=(file_table_entry){.fd=ft->next_id++,.base=file,.seek_offset=0};

	sorted_list_insert(ft->entries,fe);
	return fe->fd;
}
int remove_file_table_entry(file_table* ft,int fd){
	file_table_entry* file = sorted_list_get(ft->entries, (void *)(uint64_t)fd);
    vfs_close(file->base);
	sorted_list_remove(ft->entries,file);
	free(file);
    return 0;
}