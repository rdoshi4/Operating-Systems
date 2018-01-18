#include <sys/defs.h>
#include <sys/gdt.h>
#include<sys/kprintf.h>

inline unsigned long sysInLong(unsigned short port)
{
   unsigned long result;
   __asm__ __volatile__("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
   return result;
}
inline void sysOutLong(unsigned short port, unsigned long data)
{
   __asm__ __volatile__("outl %%eax, %%dx" : : "d" (port), "a" (data));
}
uint16_t checkAhci (uint8_t bus, uint8_t slot,uint8_t offset)
 {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = 0;
    uint16_t tmp = 0;
    uint8_t  tmp1=0;
    uint8_t tmp2=0;
    /* create configuration address as per Figure 1 */
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    sysOutLong (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	if(tmp!=0xffff)
	{ 
          tmp1=(tmp >>8);
	  tmp2=(tmp & 0xff);	
   	if(tmp1==1 && tmp2==6)
             kprintf("found!");
	 //kprintf(" %x %x\n ",tmp1,tmp2);
	}
//return (tmp);
return 0;
 
}
void checkAllBuses(void) {
     uint8_t bus;
     uint8_t device;
    // uint8_t offset1 = 0x02;
     uint8_t offset2 = 0xA;
    // uint16_t i;
     uint16_t j;
     for(bus = 0; bus < 256; bus++) 
         for(device = 0; device < 32; device++)
	    j= checkAhci(bus, device, offset2);
 
	    while(j);
 }

