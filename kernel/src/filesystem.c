#include "filesystem.h"
#include "vfs.h"
#include "process.h"

//ANALOG FOR INTERRUPT SERVICE ROUTINES

extern vfs_node root;
extern process* current_process;

char *getcwd(char *buf, size_t size){
	if(vfs_get_full_filepath(current_process->working_dir,buf,size)==-1){
		return NULL;
	}
	return buf;
}
int mkdir(const char *pathname, mode_t mode){
	vfs_node* cur=current_process->working_dir;
    return (int)vfs_create_folder(cur,pathname,0);
}

int chdir(const char *path){
	vfs_node* cur=current_process->working_dir;
	vfs_node* to=vfs_navigate_to(cur, path, false);
	if(to){
		current_process->working_dir= to;
		return 0;
	}
	return -1;
}

int open(const char *pathname, int flags){
	vfs_node* file= vfs_open(current_process->working_dir,pathname);
	if(!file)
		return -1;
	return create_file_table_entry(current_process->process_file_table,file);
}

int close(int fd){
	file_table_entry* file = sorted_list_get(current_process->process_file_table->entries, (void *)(uint64_t)fd);
    vfs_close(file->base);
	remove_file_table_entry(current_process->process_file_table,fd);
	return 0;
}
ssize_t read(int fd, void *buf, size_t count){
	return file_read(current_process->process_file_table,fd,buf,count);
}

ssize_t write(int fd, const void *buf, size_t count){
	return file_write(current_process->process_file_table,fd,buf,count);
}


int fstat(int fd, struct stat *buf){
	return file_fstat(current_process->process_file_table,fd,buf);
}

off_t lseek(int fd, off_t offset, int whence){
	file_seek(current_process->process_file_table,fd,offset,whence);
	return 0;
}
int mkfifo (const char *pathname, mode_t mode){
    vfs_node* cur=current_process->working_dir;
    return (int)vfs_create_pipe(cur,pathname);
}