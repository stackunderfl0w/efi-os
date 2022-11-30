#pragma once
#include "vfs.h"
#include "sorted_list.h"

#define EOF (-1)

typedef long int off_t;
#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */


typedef struct{
	int fd;
	//vfs node representing file
	vfs_node* base;
	int64_t seek_offset;
}file_table_entry;


typedef struct{
	//file_table_entry entries[256];
	sorted_list* entries;
	//tmp
	int next_id;
}file_table;

//only st_size implemented for now
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

int64_t file_read(file_table* ft,int fd, uint8_t* buf, uint64_t len);

int64_t file_read(file_table* ft,int fd, uint8_t* buf, uint64_t len);

int64_t file_write(file_table* ft,int fd, const uint8_t* buf, uint64_t len);

int64_t file_seek(file_table* ft,int fd, off_t offset, int whence);

int file_fstat(file_table* ft,int fd, struct stat *stat_buf);

int create_file_table_entry(file_table* ft, vfs_node* file);

int remove_file_table_entry(file_table* ft,int fd);
