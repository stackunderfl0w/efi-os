#include "vfs.h"
#include "sorted_list.h"
#include "ata-pio.h"
#include "file_table.h"
#include "fat.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"

int cmp_vfs_node_by_filename(vfs_node* first, vfs_node* second){
	return strcmp(first->name,second->name);
}

vfs_node* vfs_create_root(uint64_t root_drive){
	vfs_node* root=kmalloc(sizeof(vfs_node));
	strcpy(root->name,"/");
	root->size=0;
	root->flags=VFS_DIRECTORY|VFS_MOUNT|VFS_ROOT;
	root->drive_id=root_drive;
	root->children= create_sorted_list((int (*)(void *, void *)) cmp_vfs_node_by_filename);
	root->parent=root;
	return root;
}

void vfs_free(vfs_node* root){
	if(root->flags&VFS_DIRECTORY){
		for (int i = 0; i < root->children->size; ++i) {
			vfs_free(root->children->data[i]);
		}
		sorted_list_free(root->children);
	}
	free(root);
}

void vfs_mount_drive(uint64_t drive_id, char* path){

}

void vfs_create_file(char* filename){

}

vfs_node* vfs_get_single_entry_from_dir(vfs_node* dir, const char* filename){
	if(!(dir->flags&VFS_DIRECTORY))
		return NULL;
	if(!strcmp(filename,"."))
		return dir;
	if(!strcmp(filename,".."))
		return dir->parent;

	for (int i = 0; i < dir->children->size; ++i) {
		vfs_node* cur=dir->children->data[i];
		if(!strcasecmp(cur->name,filename))
			return cur;
	}
	return NULL;
}
vfs_node* vfs_get_entry_from_dir(vfs_node *cur, const char* filepath){
	char cur_file_name[256]={0};

	const char* f=filepath;
	while(f){
		while(*f=='/')
			f++;
		char* tok_end =strchr(f,'/');
		strncpy(cur_file_name,f,tok_end?tok_end-f:256);
		f=tok_end;
		cur = vfs_get_single_entry_from_dir(cur, cur_file_name);
		if (!cur)
			return NULL;
	}
	return cur;
}

vfs_node * vfs_open_file(vfs_node *cur, const char* filepath){
	cur= vfs_get_entry_from_dir(cur,filepath);
	if(!cur->open_references){
		//cur->data_cache=cur->seek_head=kmalloc((cur->size&0x1ff)+512);
		cur->data_cache=load_fat_cluster_chain(cur->location);
	}
	cur->open_references++;
	return cur;
}

void vfs_close_file(vfs_node* file){
	if(file->open_references>0){
		if(--file->open_references==0){
			//write to disk

			free(file->data_cache);
		}
	}
}

uint64_t vfs_create_pipe(char* filename){

}

uint64_t vfs_close_pipe(char* filename){

}
///todo check for file boundaries and add error codes
int64_t vfs_file_read(vfs_node* file, void *buf,size_t offset,size_t count){
	if(offset+count<=file->size){
		memcpy(buf,file->data_cache+offset,count);
		return (int64_t)count;
	}
	else{
		memcpy(buf,file->data_cache+offset,file->size-offset);
		//return 0 for end of file
		return 0;
	}
}
///todo
int64_t vfs_file_write(vfs_node* file, const void *buf,size_t offset, size_t count){
	//check if data cache is too small
	if(offset+count>ROUND_4K(file->size)){
		//map new pages
		//or in testing just realocate
		krealloc(file->data_cache,ROUND_4K(file->size)+4096);
	}
	if(offset+count>file->size){
		file->size=offset+count;
	}
	memcpy(file->data_cache+offset,buf,count);
	return (int64_t)count;
}


int vfs_get_full_filepath(vfs_node* node, char* buf, uint64_t max_size){
	int index=0;
	if(!(node->flags&VFS_ROOT)){
		index=vfs_get_full_filepath(node->parent,buf,max_size);
	}
	else{
		buf[index]='/';
		buf[index+1]=0;
		return 1;
	}
	if((index+1+strlen(node->name)<max_size)){
		if(!(node->parent->flags&VFS_ROOT)){
			buf[index]='/';
			index++;
		}
		strcpy(buf+index,node->name);
	}
	return (int)(index+ strlen(node->name));
}
void print_vfs_recursive(vfs_node* dir, int level){
	for (int i = 0; i < level; ++i) {
		kprintf("\t");
	}

	kprintf("\033[3%um%s\n",dir->flags&VFS_DIRECTORY?2:3,dir->name);
	if(dir->flags&VFS_DIRECTORY){
		for (int i = 0; i <dir->children->size ; ++i) {
			print_vfs_recursive(dir->children->data[i],level+1);
		}
	}
}

void vfs_recursive_populate(vfs_node* root, char* path, int max_level){

	fat_populate_vfs_directory(root,path);
	if(!max_level)
		return;
	uint64_t pathlen= strlen(path);
	char* dir=kmalloc(pathlen+257);
	strcpy(dir,path);
	for (int i = 0; i < root->children->size; ++i) {
		if(((vfs_node*)root->children->data[i])->flags&VFS_DIRECTORY){
			vfs_node* cur=root->children->data[i];
			strcpy(dir+pathlen,cur->name);
			strcpy(dir+ strlen(dir),"/");
			//kprintf("%s\n",dir);
			vfs_recursive_populate(cur,dir,max_level-1);
		}
	}
	free(dir);
}