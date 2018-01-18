#include <sys/keyboardHandler.h>
#include<sys/kprintf.h>
#include<sys/tarfs.h>
//#include<port.h>
char scanC[256]={0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,0,0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','?'};
char command_buffer[100];
//char command[100];
volatile int buf_count=0;
volatile  int done_read=0;
static uint64_t keyboardFunction()
{
  uint8_t scancode;
  char display;
  //__asm__  volatile( "inb %1, %0" : "=a"(scancode) : "Nd"(0x60));
	scancode=inb(0x60);
 //kprintf("\n%d",scancode);
 if(scancode & 0x80)
 {
        if(scancode==(128+42) || scancode==(128+54))
                shift_pressed=0;
        if(scancode==(128+29))
	{
                ctrl_pressed=0;
	}
 }
 else  if(scancode==54 || scancode==42)
        shift_pressed=1;
 else  if(scancode==29)
        ctrl_pressed=1;
 else
 {
        if(scancode==46 && ctrl_pressed==1)
        {
                printEscape();
        }
   	//display=scanC[scancode];
	else
	{
		if(scancode==28)
                {
                        done_read=1;
                        printEnter();
                        io_wait();
                        //strcpy(command_buffer, command);
                        //kprintf("command :%s\n", command);
                        return 0;
                }
                else if(scancode==14)
                {
                        keyRemove();
                        command_buffer[buf_count]='\0';
                        buf_count--;
                }
                else
                {
                        display=scanC[scancode];
                        if(shift_pressed)
                                display=scanC[scancode+128];
                        command_buffer[buf_count++]=display;
                        printKey(display);
                }
	}
  }
return 0;
}
void keyboardInitialize()
{
  handleRegInterrupt(33, keyboardFunction);
}
