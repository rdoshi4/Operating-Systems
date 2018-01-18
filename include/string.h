#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED

extern void strcat(char *a ,char *b);
extern int strcmp(char *a,char *b);
extern void memset(void *addr,int val,int len);
extern int stoi(char *a);
char * strtok(char * a, char * delimiter);
extern int strlen(char *a);
extern int  strcpy(char *src,char *dst);
extern int memcpy(volatile void *dest,volatile void *src,int n);
#endif
