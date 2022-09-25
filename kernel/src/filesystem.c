//
// Created by pat on 9/22/22.
//

#include "filesystem.h"
#include "vfs.h"
#include "thread.h"
//ANALOG FOR INTERRUPT SERVICE ROUTINES

extern vfs_node root;
extern process* current_process;

char *getcwd(char *buf, size_t size){
	if(vfs_get_full_filepath(current_process->working_dir,buf,size)==-1){
		return NULL;
	}
	return buf;
}
int chdir(const char *path){
	vfs_node* cur=current_process->working_dir;
	current_process->working_dir= vfs_get_entry_from_dir(cur,path);
	return 0;
}

int open(const char *pathname, int flags){
	vfs_node* file= vfs_open_file(current_process->working_dir,"/resources/resourcesresources/config.txt");
	return create_file_table_entry(current_process->process_file_table,file);
}

int close(int fd){
	file_table_entry tmp={fd,0};
	file_table_entry* file = sorted_list_get(current_process->process_file_table->entries, (void *)&tmp);
	vfs_close_file(file->base);
	remove_file_table_entry(current_process->process_file_table,fd);
}
ssize_t read(int fd, void *buf, size_t count){
	return file_read(current_process->process_file_table,fd,buf,count);
}

int fstat(int filedes, struct stat *buf){
	return file_fstat(current_process->process_file_table,filedes,buf);
}
