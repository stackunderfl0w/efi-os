#include "vfs.h"
#include "sorted_list.h"
#include "ata-pio.h"
#include "file_table.h"
#include "fat.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"
#include "circular_buffer.h"
int cmp_vfs_node_by_filename(vfs_node* first, vfs_node* second){
	return strcmp(first->name,second->name);
}
int search_cmp_vfs_node(vfs_node* first, char* second){
	return strcmp(first->name,second);
}

vfs_node* vfs_create_root(uint64_t root_drive){
	vfs_node* root=kmalloc(sizeof(vfs_node));
	strcpy(root->name,"/");
	root->size=0;
	root->flags=VFS_DIRECTORY|VFS_MOUNT|VFS_ROOT;
	root->drive_id=root_drive;
	root->children= create_sorted_list((int (*)(void *, void *)) cmp_vfs_node_by_filename,(int (*)(void *, void *)) search_cmp_vfs_node);
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
vfs_node* vfs_open(vfs_node *cur, const char* filepath){
    cur= vfs_get_entry_from_dir(cur,filepath);
    if(!cur->open_references){
        if(cur->flags&VFS_PIPE){
            cur->data_cache=(uint8_t*)cb_init(4096);
        }
        else{
            cur->data_cache=load_fat_cluster_chain(cur->location);
        }
    }
    cur->open_references++;
    return cur;
}

void vfs_close(vfs_node* file){
	if(file->open_references>0){
		if(--file->open_references==0){
			//write to disk

			free(file->data_cache);
		}
	}
}

uint64_t vfs_create_pipe(vfs_node *cur, const char* filename){
	vfs_node* n=kmalloc(sizeof(vfs_node));
	strncpy(n->name,filename,256);
	n->size=0;
	n->flags=VFS_FILE|VFS_PIPE|VFS_VOLATILE;
	n->open_references=0;
	n->parent=cur;
	sorted_list_insert(cur->children,n);
	return 0;
}
///todo check for file boundaries and add error codes
int64_t vfs_file_read(vfs_node* file, void *buf,size_t offset,size_t count){
    if(offset+count>file->size){
        count=file->size-offset;
    }
    memcpy(buf,file->data_cache+offset,count);
    return (int64_t)count;
}
//the file size of a pipe is how many characters are in the pipe. Reading from it removes the input.
int64_t vfs_pipe_read(vfs_node* file, void *buf,size_t count){
    return (int64_t)cb_pop((circular_buffer*)file->data_cache,buf,count);
}
int64_t vfs_pipe_write(vfs_node* file, const void *buf,size_t count){
    return (int64_t)cb_push((circular_buffer*)file->data_cache,buf,count);
}
///todo
int64_t vfs_file_write(vfs_node* file, const void *buf,size_t offset, size_t count){
	//check if data cache is too small
	if(offset+count>ROUND_4K(file->size)){
		//map new pages
		//or in testing just realocate
        void* new_cache=krealloc(file->data_cache,ROUND_4K(file->size)+4096);
        if(new_cache){
            file->data_cache=new_cache;
        }
        ///todo else some kind of future memory panic
	}
	if(offset+count>file->size){
		file->size=offset+count;
	}
	memcpy(file->data_cache+offset,buf,count);
	return (int64_t)count;
}
int64_t vfs_read(vfs_node* file, void *buf,size_t offset,size_t count){
    if(file->flags&VFS_PIPE){
        return vfs_pipe_read(file,buf,count);
    }
    if(file->flags&VFS_FILE){
        return vfs_file_read(file,buf,offset,count);
    }
    return -1;
}
int64_t vfs_write(vfs_node* file, const void *buf,size_t offset, size_t count){
    if(file->flags&VFS_PIPE){
        return vfs_pipe_write(file,buf,count);
    }
    if(file->flags&VFS_FILE){
        return vfs_file_write(file,buf,offset,count);
    }
    return -1;
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
		strncpy(buf+index,node->name,256);
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
			strncpy(dir+pathlen,cur->name,256);
			strcpy(dir+ strlen(dir),"/");
			//kprintf("%s\n",dir);
			vfs_recursive_populate(cur,dir,max_level-1);
		}
	}
	free(dir);
}