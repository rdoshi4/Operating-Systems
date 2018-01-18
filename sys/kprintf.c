#include <sys/kprintf.h>
#include<stdarg.h>
#include<sys/port.h>
#include<string.h>
//#include "sys/elf64.h"

volatile char *video = (volatile char*)0xffffffff800b8000;
char hexarray[20]="0123456789abcdef";
int count;
int line[25];
void scroll_up();

int row_number=24;
int column_number=80;
char *startingAddrOfVgaBuf = (char *)0xFFFFFFFF800B8000;
char *endAddrOfVgaBuf = (char *)0xFFFFFFFF800B8F00UL;

uint64_t getRowNumber()
{
  return (((uint64_t)video - (uint64_t)startingAddrOfVgaBuf)/160);
}

uint64_t getColumnNumber()
{         
  return ((((uint64_t)video - (uint64_t)startingAddrOfVgaBuf)/2)%80);
}

void scroll()
{
  int r = getRowNumber();
  int c = getColumnNumber();
  if(r >= 23 && c < 30)
  {
    uint64_t begin = (uint64_t)startingAddrOfVgaBuf + 160;
    uint64_t end = (uint64_t)startingAddrOfVgaBuf;
    uint64_t temp = (23 *160);
    memcpy((void *) end, (void *) begin, temp);
    begin = (uint64_t) startingAddrOfVgaBuf + temp;
    temp = 160;
    memset((void *) begin, 0, temp);
    int rNum = getRowNumber() - 1;
    int cNum = getColumnNumber();
    uint64_t temp2 = (uint64_t)startingAddrOfVgaBuf + (rNum * 160 + cNum * 2);
    video = (char *) temp2;
  }
}

int printOnScreen(const char * str, int len)
{
  int i=0;
  while(*str && i <len)
  {
    scroll();
    *video = *str;
    video += 2; i++; ++str;
  } return 1;
}

int printChar(char c)
{
  scroll();
  *video = c; video +=2;
  return 1;
}

int printString(char* s)
{
  int length = strlen(s);
  printOnScreen(s, length);
  return 1;
}

int printHex(int h)
{
  int i=0;
  char hArray[33];
  char *hArrayPtr = &hArray[sizeof(hArray)-1];
  *hArrayPtr ='\0';
  while(h!=0)
  {
    *--hArrayPtr = hexarray[h%16];
    h/=16;
    i++;
  }
  printOnScreen(hArrayPtr, i);
  return 1;
}

int printPointer(void *pAddr)
{
  int i=0;
  unsigned long long a = (unsigned long long) pAddr;
  char pArray[33];
  char *pArrayPtr =&pArray[sizeof(pArray)-1];
  *pArrayPtr = '\0';
  while(a!=0)
  {
    *--pArrayPtr=hexarray[a % 16];
    a /= 16; i++;
  }
  printOnScreen(pArrayPtr, i);
  return 1;
}

int printCharacter(char c)
{
  char temp[2];
  temp[0]=c; temp[1]='\0';
  int l = strlen(temp);
  printOnScreen(temp, l);
  return 1;
}

int printInteger(int d)
{
  int i=0, j=0, k=5, l;
  char temp1, temp2[10];
  char* temp3 = temp2;
  while(k!=0)
  {
    *temp3++=0; k--;
  }
  if(d ==0)
  {
    temp2[i]='0';
  }
  while(d>0)
  {
    temp2[i++]=('0'+(d % 10)); d/=10;
  }
  for(j=0;j<(i/2);j++)
  {
    temp1 = temp2[j]; temp2[j] = temp2[i-j-1]; temp2[i-j-1] = temp1;
  }
  l = strlen(temp2);
  printOnScreen((const char*) temp2, l);
  return 1;
}

int escape_characters(const char *c)
{
  if(*c == '\b')
  {
    if(((uint64_t)video) >= (uint64_t)startingAddrOfVgaBuf)
    {
      char* character = (char*)(((uint64_t)video) - 2);
      if(*(character-6) == ' ' && *(character-4) == ' ' && *(character-2) == ' ' && *character == ' ')
      {
        video = (char*)((uint64_t)video -10);
        printChar(' ');
        video = (char*)((uint64_t)video -2);
      }
      else
      {
        video = (char*)((uint64_t)video -2);
        printChar(' ');
        video = (char*)((uint64_t)video -2);
      }
    }
    return 1;
  }
  else if(*c == '\t')
  {
    video = (char *) ((uint64_t)video + 8);
    scroll();
    return 1;
  }
  else if(*c == '\n')
  {
    scroll();
    int row = getRowNumber() + 1;
    uint64_t updatedAddr = (uint64_t)startingAddrOfVgaBuf + (row * 160);
    video = (char *) updatedAddr;
    //scroll();
    return 1;
  }
  else
  {
    return 0;
  }
}

int backSpace(const char * b)
{
  char *p=NULL;
  if(*b =='\b')
  {
    video = video-2;
    *video = *p;
    if(column_number ==0)
    {
      column_number=79; row_number--;
    }
    else
    { column_number--;
    } return 1;
  }
  else
  { return 0;}
}

void kprintf(const char *fmt, ...)
{
  int finish=0, var;
  const char* temp;
  va_list args;
  va_start(args, fmt);
  while(*fmt!=0)
  {
    if(*fmt== '%')
    {
      temp = fmt; ++fmt;
      if(*fmt=='\0') { break; }
      switch(*fmt)
      {
        case 'c':
            printCharacter(va_arg(args, int)); finish++; break;
        case 'd':
            printInteger(va_arg(args,int)); finish++; break;
        case 's':
            var = printString(va_arg(args, char*)); finish+=var; break;
        case 'x':
            printHex(va_arg(args, int)); finish++; break;
        case 'p':
            printPointer(va_arg(args, void*)); finish++; break;
        default:
            printOnScreen(temp, 1); printOnScreen(fmt, 1); break;
      }
    }
    else
    {
      fmt = fmt + escape_characters(fmt) + backSpace(fmt);
      if(*fmt != '\0')
      { printOnScreen(fmt, 1); }
      else { break; }
      finish++;
    } fmt++;
  } va_end(args);
}

void printnewline()
{
int x=count%160;
//int i=0;
int y=(count/160),i=0;
if(y==23)
{
//	scroll();
	scroll_up();
	count=count-160;
	video = (volatile char*)0xffffffff800b8000;
	for(i=0;i<count;i++)
		video++;
	return;
}	
line[y]=x;
//kprintf("%d",line[y]);
count=count+(160-x);
video=(volatile char*)0xffffffff800b8000;
for(i=0;i<count;i++)
{
	*video++;
}
}
void printchar(int c)
{
       *video++ = c;
       *video++ = 9;
	count=count+2;
}
void printstr(char *s)
{
int i=0;
        while(s[i]!='\0')
        {
                *video++ = s[i];
                *video++ = 9;
		count=count+2;
		i++;
        }
}
void printptr(unsigned long long  ptr)
{
	//unsigned long long addr=(unsigned long long)ptr;
	int i=0;
	char str[100]="0x";
	int rem;
	while(ptr)
        {

                rem=ptr%16;
                str[i]=hexarray[rem];
                ptr=ptr/16;
                i++;
        }
	str[i++]='x';
	str[i]='0';	
    while( i >= 0 )
    {
        *video++ =str[i];
        *video++ = 9;
        i--;
	count=count+2;
    }	
}
void printhex(int hex)
{
	
	char str[100]="0x";
	//str[0]='\0';
	int rem,i=0;
	while(hex)
	{
		
		rem=hex%16;
		str[i]=hexarray[rem];
		hex=hex/16;
		i++;
	}
	
//	*video--;
	i=i-1;
    while( i >= 0 )
    {
        *video++ =str[i];
        *video++ = 9;
	count=count+2;
	i--;
    }

}
int k = 0;
void printKey(char c)
{
	int j=(22*160)+14,i;
        volatile char *display = (volatile char*)0xffffffff800b8000;
        if(k>140)
        {
                k=0;
        }
        for(i=0;i<(j+k);i++)
        {
                display++;
        }
	*display++=c;
	//*display=9;
        k+=2;
}
void clearConsole()
{
	int l = (24*160);
	volatile char *display = (volatile char*)0xffffffff800b8000;
	for(int i=0;i<l;i++)
        {
		*display++=' ';
		*display++;
	}
}
void keyRemove()
{
        int j=(22*160)+14,i;
        volatile char *display = (volatile char*)0xffffffff800b8000;
        for(i=0;i<(j+k-2);i++)
                display++;
        *display++=' ';
        *display=9;
        k-=2;
}
void printEnter()
{
    uint64_t begin = (uint64_t)startingAddrOfVgaBuf + 160;
    uint64_t end = (uint64_t)startingAddrOfVgaBuf;
    uint64_t temp = (23 *160);
    memcpy((void *) end, (void *) begin, temp);
    begin = (uint64_t) startingAddrOfVgaBuf + temp;
    temp = 160;
    memset((void *) begin, 0, temp);
    int rNum = getRowNumber() - 1;
    int cNum = getColumnNumber();
    uint64_t temp2 = (uint64_t)startingAddrOfVgaBuf + (rNum * 160 + cNum * 2);
    video = (char *) temp2;
    k=0;
}
void printint(int val)
{
	char str[100];
	str[0]='\0';
	int rem,i=0;
	while(val)
	{
		rem=val%10;
		str[i]=rem +'0';
		val=val/10;
		i++;
	}
//	*video--;
	i=i-1;
	while(i>=0)
	{
		*video++ =str[i];
        	*video++ = 9;
        	count=count+2;
        	i--;

	}
}
void printEscape()
{
	int j=(23*160)+10,i;
        volatile char *display = (volatile char*)0xffffffff800b8000;
        for(i=0;i<j;i++)
                display++;
	*display++='^';
	*display++=9;
	*display++='C';
	*display=9;
}
/*
void keyRemove()
{
	int j=(23*160)+11,i;
        volatile char *display = (volatile char*)0xffffffff800b8000;
        for(i=0;i<j;i++)
                display++;
	*display=' ';
}
*/
void scroll_up()
{
	int i,j,k;
	volatile char *display = (volatile char*)0xffffffff800b8000;
	for(i=0;i<24;i++)	
	{
		display=(volatile char*)0xffffffff800b8000;
		/*for(j=0;j<line[i];j=j+2)
		{
			*(video +160*i +j)=' ';
		}*/
		for(j=0;j<line[i+1];j=j+1)
		{	
			*(display+(i)*160+j) = *(display +(i+1)*160+j);
		}
		for(k=j;k<160;k=k+2)
			*(display+(i)*160+k)=' ';
	//	count=count-2*160;
	
	}
}
void printTime(int c)
{
	int j=23*160,i;
	volatile char *display = (volatile char*)0xffffffff800b8000;
	for(i=0;i<j;i++)
		display++;
	 char str[100];
        str[0]='\0';
	i=0;
        int rem;
        while(c)
        {
                rem=c%10;
                str[i]=rem +'0';
                c=c/10;
                i++;
        }
//      *video--;
        i=i-1;
        while(i>=0)
        {
                *display++ =str[i];
                *display++ = 2;
                i--;
        }
//	*display++=c;
//	*display=9;
}
/*
void kprintf(const char *fmt,...)
{
if(line[24]==159)
	scroll_up();
//scroll();
int i=0;
//char c='d';
va_list valist;
va_start(valist,fmt);
while(fmt[i]!='\0')
{
	if(fmt[i]=='%')
	{
		
		if(fmt[i+1]=='x')
			printhex(va_arg(valist,int));
		if(fmt[i+1]=='p')
			printptr(va_arg(valist,unsigned long long));
		if(fmt[i+1]=='d')
			printint(va_arg(valist,int));
		if(fmt[i+1]=='c')
			printchar(va_arg(valist,int));
                if(fmt[i+1]=='s')
                        printstr(va_arg(valist,char*));
		i=i+2;
		continue;
	}
	if(fmt[i]=='\n')
	{
//                printNewLine();
		printnewline();	
		i++;
		continue;
	}
	*video++=fmt[i];
	*video++=9;
	count=count+2;
	i++;
}
}
*/
