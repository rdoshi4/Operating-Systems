
#include "stdio.h"
#include<sys/defs.h>
 struct FILE
{
    char*   _ptr;
    int _cnt;
    char*   _base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char*   _tmpfname;
};

char c[100];
int syscall=3;
int fd=0;
int count=1;
int getchar()
{
        //char c;
        int err;
        //int syscall=0,fd,count;
        //fd=0;
        //count=1;
        __asm__ volatile("int $0x80":"=a"(err):"a"(syscall),"b"(fd),"c"(c),"d"(count):"memory");
        //if(err<0)
                //puts("error in reading character");
        return c[0];
}

int putchar(int ch)
{
  // write character to stdout
	c[0]=ch;
        //__asm__ volatile("int $0x80"::"a"(4),"b"(0),"c"(c),"d"(1):"memory");
	__asm volatile("int $0x80"::"c"(4),"b"(ch),"d"(1));
  return ch;
}

int puts(const char *s)
{
//  for( ; *s; ++s) if (putchar(*s) != *s) return -1;
 // return (putchar('\n') == '\n') ? 0 : -1;
    __asm volatile("int $0x80"::"c"(4),"b"(s),"d"(1000));
	return 1000;
}
/*struct FILE *popen(const char *command, const char *mode)
{
struct FILE *fp;
fp=0;
return fp;
}*/
void read(int fd,char *buf,int length)
{
	__asm volatile("int $0x80"::"c"(3),"b"((unsigned long) buf), "d"(fd));
}
void write(char *argv[])
{
	uint64_t b= (uint64_t )argv;
	__asm volatile("int $0x80"::"c"(4),"b"(b),"d"(100));
}
int open(char *arg)
{
int fd;
	__asm volatile("int $0x80":"=a"(fd):"c"(2), "b"((unsigned long) arg));
	return fd;
}
