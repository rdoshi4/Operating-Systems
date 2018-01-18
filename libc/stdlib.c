#include "stdlib.h"
#include "string.h"
char **envvar;
char str[1000]="/home/sharisha/bin";
void *malloc(int size)
{
  uint64_t addr;
  __asm volatile("int $0x80":"=a"(addr):"c"(9),"b"(size));
  return (void *)addr;
}
//char env[1000];
/*char *getenv(const char *varName)
{
  return env;
}
int setenv(const char *envname, const char *envval, int overwrite)
{
  return 0;
}*/

char *getenv( char *varName)
{
//return envvar[8];
/*int i=0,j;
char envvar[100];
while(envp[i]!=NULL)
{
        for(j=0;envp[i][j]!='=';j++)
        {
                envvar[j]=envp[i][j];
        }
        if(!strcmp(envvar,varName))
                return envp[i];
i++;
}
return envp[8];*/
if(strcmp(varName,"PATH"))
{
	
	return str;
}
else
{
	ps1=str;
	return ps1;
}

}

int setenv(char *envname,char *envval, int overwrite)
{
        return 0;
}
void sleep(char *argv[])
{
	uint64_t b= stoi((char *)argv);
	__asm volatile("int $0x80"::"c"(35),"b"(b));
}
void kill(char *argv[])
{
	uint64_t b= stoi((char *)argv);
	__asm volatile("int $0x80"::"c"(62),"b"(b));
}
void wait()
{
	__asm volatile("int $0x80"::"c"(247));
}
void yield()
{
        __asm volatile("int $0x80"::"c"(24));
}
