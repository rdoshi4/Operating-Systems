#include "string.h"

void strcat(char *a ,char *b)
{
        int i,j;
        for(i=0;a[i]!='\0';i++)
        {
        }
        for(j=0;b[j]!='\0';j++)
        {
                a[i+j]=b[j];
        }
        a[i+j]='\0';
}
int strlen(char *a)
{
	int i=0;
	while(a[i]!='\0')
	i++;
	return i;
}
int stoi(char *a)
{
	int i=0;
	int val=0;
 	while(a[i]!='\0') 
 	{	 
		val=val*10 + a[i]-'0';
		i++;
 	}
return val;
}
char * strtok (char * a, char * delimiter)
{
        int i=0; int l=strlen(delimiter);
        char * p=0;
        static char * key=0;
        if(!key && !a) { return 0; }
        if(!key && a) { key=a; }
        p=key;
        while(1)
        {
                for(i=0; i<l; i++)
                {
                        if(*p==delimiter[i]) {  p++; break; }
                }
                if(i==1) { key=p; break; }
        }
        if(*key=='\0') { key=0; return key; }
        while(*key !='\0')
        {
                for(i=0; i<l; i++)
                {
                        if(*key==delimiter[i]) { *key='\0'; break; }
                }
                key++;
                if(i<l) { break; }
        } return p;
}
int strcmp(char *a,char *b)
{
int j,err=0;
if(a[0]==0 && b[0]==0)
        return 0;
if(a[0]==0 || b[0]==0)
        return -1;
for(j=0;a[j]!='\0';j++)
{
        if(a[j]!=b[j]) { err=-1; break;  }
}
if(err!=-1)
{
        if(a[j]!=b[j]){ return -1; }
        else
            return 0;
}
return err;
}
void memset(void *addr,int val,int len)
{
        unsigned char *read=addr;
        int i;
        for(i=0;i<len;i++)
        {
                *read=val;
                read++;
        }

}
int  strcpy(char *src,char *dst)
{
	int i=0;
	while(src[i]!='\0')
	{	
		dst[i]=src[i];
		i++;
	}
	dst[i] = '\0';
	return i;
}
int memcpy(volatile void *dest,volatile void *src,int n)
{
	 const unsigned  char *csrc =(unsigned char *)src;
	 unsigned char *cdest =(unsigned char *)dest;
	int i;
	for(i=0;i<n;i++)
		cdest[i]=csrc[i];
 return n;
}

