#pragma once
#include "vfs.h"
#include "sorted_list.h"

typedef struct{
	int fd;
	//vfs node representing file
	vfs_node* base;
}file_table_entry;


typedef struct{
	//file_table_entry entries[256];
	sorted_list* entries;
	//tmp
	int next_id;
}file_table;

struct stat{
	uint64_t	st_dev;         /* ID of device containing file */
	uint64_t	st_ino;         /* Inode number */
	uint64_t	st_mode;        /* File type and mode */
	uint64_t	st_nlink;       /* Number of hard links */
	uint64_t	st_uid;         /* User ID of owner */
	uint64_t	st_gid;         /* Group ID of owner */
	uint64_t	st_rdev;        /* Device ID (if special file) */
	uint64_t	st_size;        /* Total size, in bytes */
	uint64_t	st_blksize;     /* Block size for filesystem I/O */
	uint64_t	st_blocks;      /* Number of 512B blocks allocated */
};


file_table* init_file_table();

uint64_t file_read(file_table* ft,int fd, uint8_t* buf, uint64_t len);

int file_fstat(file_table* ft,int fd, struct stat *statbuf);

int create_file_table_entry(file_table* ft, vfs_node* file);

int remove_file_table_entry(file_table* ft,int fd);
