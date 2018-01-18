#include<sys/kprintf.h>
#include<signal.h>
#include<unistd.h>
char scan[256]={0,27,'1','2','3','4','5','6','7','8','9','0','-','=','\b','\t','q','w','e','r','i','y','u','i','o','p','[',']','\n',0,'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z','x','c','v','b','n','m',',','.','/',0,0,0,0,0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,'A','S','D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V','B','N','M','<','>','?'};

/*void exitcode()
{
	exit(0);
}*/
void key_handler()
{
uint8_t scancode;
char display;
__asm__  volatile( "inb %1, %0" : "=a"(scancode) : "Nd"(0x60));
if(scancode & 0x80)
{
	if(scancode==(128+42) || scancode==(128+54))
		shift_pressed=0;
	if(scancode==(128+29))
		ctrl_pressed=0;
}
else if(scancode==54 || scancode==42)
	shift_pressed=1;
else if(scancode==29)
	ctrl_pressed=1;
else
{
	if(scancode==46 && ctrl_pressed==1)
	{
		kprintf("%s","^C");
	}
	display=scan[scancode];
	if(shift_pressed)
		display=scan[scancode+128];
	printKey(display);
}
}
