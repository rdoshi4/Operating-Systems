#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/phymem.h>
#include <sys/virmem.h>
#include <sys/process.h>
#include <sys/timerInterrupt.h>
#include <sys/keyboardHandler.h>
//#include <sys/isr.h>
#include <sys/initializeIDT.h>
#include<sys/elf64.h>
//#include <sys/pci.h>
//#include <sys/pci1.h>
//#include <sys/initpci.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
extern int  enable_paging;
extern uint64_t * freelist;
extern uint64_t *pml4;

void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  uint64_t  max_length=0;
  uint64_t max_base=0;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
     //kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
	make_free_list((uint64_t)physbase,(uint64_t)physfree,smap->base, smap->base+smap->length);
	if((smap->length)> max_length)
	{
		max_length=(uint64_t)smap->length;
		max_base=(uint64_t)smap->base;
	}
    }
  }
  uint64_t newphysfree=(uint64_t)physfree + 0x1000;
  setup_paging(newphysfree, max_base+max_length);
  
  set_cr3();
  enable_paging= 1;
  init_tarfs();
	initializeIDT();
	initial_kernel_thread();
while(1);
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp1, *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) *temp2 = 7 /* white */;
  __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt();
//  initializeIDT();
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );
  for(
    temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1, temp2 += 2
  ) *temp2 = *temp1;
  while(1);
}
