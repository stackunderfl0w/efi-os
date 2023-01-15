#pragma once
#include "typedef.h"
#include "sorted_list.h"


#define VFS_ROOT		1<<0
#define VFS_MOUNT		1<<1
#define VFS_DIRECTORY	1<<2
#define VFS_FILE		1<<3
#define VFS_PIPE		1<<4
#define VFS_VOLATILE	1<<5
#define VFS_LINK		1<<6

typedef struct vfs_node vfs_node;

struct vfs_node{
	char name[256];
	vfs_node* parent;
	uint64_t flags;
	uint64_t size;
	uint64_t drive_id;
	uint64_t location;
	uint64_t owner;
	uint64_t permissions;

	int64_t (*write)(vfs_node* file, const void *buf,size_t offset, size_t count);
	int64_t (*read)(vfs_node* file, void *buf,size_t offset,size_t count);
	void (*open)(vfs_node *);
	void (*close)(vfs_node *);

	uint64_t block_size;
	//for now this will be fully loaded at open to make it easier to debug on linux
	uint8_t* data_cache;
	//keep track of which chunks have been cached into memory
	//includes flags for, not cached, cached, modified.
	uint8_t* segment_table;//todo
	//what part of the file is currently being writen to/read from
	uint8_t* seek_head;

	uint64_t open_references;

	//todo, refactor to use rb trees or hash tables
	sorted_list* children;
};

vfs_node* vfs_create_root(uint64_t root_drive);
void vfs_free(vfs_node* root);
void vfs_mount_drive(uint64_t drive_id, char* path);
void vfs_create_file(char* filename);

void vfs_insert_file(vfs_node* cur, char* filename, uint64_t flags, uint64_t size, uint64_t drive_id, uint64_t location, uint64_t block_size);

vfs_node* vfs_get_single_entry_from_dir(vfs_node* dir, const char* filename);
vfs_node* vfs_get_entry_from_dir(vfs_node* dir, const char* filename);


vfs_node * vfs_open(vfs_node *cur, const char* filepath);
void vfs_close(vfs_node* file);

uint64_t vfs_create_pipe(vfs_node *cur, const char* filename);
uint64_t vfs_close_pipe(vfs_node *cur, const char* filename);

uint64_t vfs_create_folder(vfs_node *cur, const char* dirname);

int64_t vfs_read(vfs_node* file, void *buf, size_t offset,size_t count);
int64_t vfs_write(vfs_node* file, const void *buf, size_t offset, size_t count);


int vfs_get_full_filepath(vfs_node* node, char* buf, uint64_t max_size);

void print_vfs_recursive(vfs_node* dir, int level);
void vfs_recursive_populate(vfs_node* root, char* path, int max_level);