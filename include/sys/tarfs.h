#ifndef _TARFS_H
#define _TARFS_H
#include<sys/process.h>
extern char _binary_tarfs_start;
extern char _binary_tarfs_end;
//volatile int done_read;

struct posix_header_ustar {
  char name[100];
  char mode[8];
  char uid[8];
  char gid[8];
  char size[12];
  char mtime[12];
  char checksum[8];
  char typeflag[1];
  char linkname[100];
  char magic[6];
  char version[2];
  char uname[32];
  char gname[32];
  char devmajor[8];
  char devminor[8];
  char prefix[155];
  char pad[12];
};
typedef struct{
  char name[100];
  int fd;
  uint64_t f_address;
  char typeflag[1];
  uint64_t size;
  uint64_t parent;
  uint64_t referenceCount;
}FILE;
extern uint64_t file_locate(char *filename);
void init_tarfs();
uint64_t  open(uint64_t file);
void dir_contents(char *dir);
uint64_t read(uint64_t fd,uint64_t buf,uint64_t length);
uint64_t write(uint64_t buf,uint64_t length);
uint64_t readdir(uint64_t buf);
uint64_t getcwd();
uint64_t chdir(uint64_t path);
uint64_t cat(uint64_t path);
#endif
