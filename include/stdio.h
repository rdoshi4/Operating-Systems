#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED

extern int getchar();
extern int putchar(int c);
extern int puts(const char *s);
extern void read(int fd,char *buf,int length);
extern void write(char *argv[]);
extern int open(char *arg);
//extern struct FILE *popen(const char *command, const char *mode);
#endif

