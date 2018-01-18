#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/defs.h>
 char *path;
 char *ps1; 

int main(int argc, char *argv[]);
void exit(int status);

//void *malloc(size_t size);
//void free(void *ptr);
extern void *malloc(int size);
extern char *getenv( char *varName);
extern int setenv( char *envname,  char *envval, int overwrite);
extern void sleep(char *argv[]);
void kill(char *argv[]);
void wait();
void yield();
#endif
