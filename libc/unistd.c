#include "unistd.h"
#include "string.h"
#include "stdlib.h"

void exit(int status)
{
//	__asm__ volatile ("int $0x80"::"a"(1),"b"(status):"memory");
	__asm volatile("int $0x80"::"c"(60),"b"(status));
}

void chdir(char *argv[])
{
	//int err;
	//__asm__ volatile("int $0x80":"=a"(err):"a"(12),"b"(path):"memory");
	//return err;
	uint64_t b= (uint64_t )argv;
        __asm volatile("int $0x80"::"c"(80),"b"((unsigned long)b));
}

char* getcwd(char * buf)
{
	//__asm__ volatile("int $0x80"::"a"(183), "b" (buf), "c" (size));
	 __asm volatile("int $0x80"::"c"(79),"b"((unsigned long) buf));
	return buf;
}

int execve( char *file_name, char *arg)
{      
	
        //__asm__ volatile("syscall"::"a"(59),"D"(command),"S"(args),"b"(env):"memory","cc");
	 __asm volatile("int $0x80"::"c"(59),"b"((unsigned long)file_name),"d"((unsigned long) arg));
  return 0;
}
void readdir(char *argv[])
{
	uint64_t b= (uint64_t )argv;
	__asm volatile("int $0x80"::"c"(78),"b"(b));
}
void ps()
{
	 __asm volatile("int $0x80"::"c"(270));
}

/*int read(int fd,char *buf,int size)
{
	int charsred;
	__asm volatile("int $0x80":"=a"(charsred):"a"(3),"b"(fd),"c"(buf),"d"(size):"memory");
	return charsred;
}

*/
/*void waitpid(int pid,int *status,int options)
{
	 __asm__ volatile("int $0x80"::"a"(247),"b"(pid),"c"(status),"d"(options):"memory");
}*/
int fork()
{
        int pid;
__asm volatile("int $0x80":"=a"(pid):"c"(57));      
        return pid;
}
