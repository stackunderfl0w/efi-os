#pragma once
#include "typedef.h"
#include "file_table.h"


//get current working directory
char *getcwd(char *buf, size_t size);
//create a directory
int mkdir(const char *pathname, mode_t mode);
//delete a directory
//int rmdir(const char *pathname);
//change working directory
int chdir(const char *path);
//make a new name for a file
//int link(const char *oldpath, const char *newpath);
//delete a name and possibly the file it refers to
//int unlink(const char *pathname);
//change the name or location of a file
//int rename(const char *oldpath, const char *newpath);
//get file status
//int stat(const char *file_name, struct stat *buf);
//change permissions of a file
//int chmod(const char *path, mode_t mode);
//change ownership of a file
//int chown(const char *path, uid_t owner, gid_t group);
//change access and/or modification times of an inode
//int utime(const char *filename, struct utimbuf *buf);
//open a directory
//DIR *opendir(const char *name);
//read directory entry
//struct dirent *readdir(DIR *dir);
//close a directory
//int closedir(DIR *dir);
//reset directory stream
//void rewinddir(DIR *dir);


//check user's permissions for a file
//int access(const char *pathname, int mode);
//open and possibly create a file or device
int open(const char *pathname, int flags);
//open and possibly create a file or device
//int creat(const char *pathname, mode_t mode);
//close a file descriptor
int close(int fd);
//read from a file descriptor
ssize_t read(int fd, void *buf, size_t count);
//write to a file descriptor
ssize_t write(int fd, const void *buf, size_t count);
//manipulate file descriptor
int fcntl(int fd, int cmd);
//get file status
int fstat(int fd, struct stat *buf);
//reposition read/write file offset
off_t lseek(int fd, off_t offset, int whence);
//duplicate a file descriptor
int dup(int oldfd);
//duplicate a file descriptor
int dup2(int oldfd, int newfd);
//create pipe
int pipe(int filedes[2]);
//make a FIFO special file (a named pipe)
int mkfifo ( const char *pathname, mode_t mode );
//set file creation mask
//mode_t umask(mode_t mask);
//associate a stream with an existing file descriptor
//FILE *fdopen (int fildes, const char *mode);
//return file descriptor of stream
//int fileno( FILE *stream);