#include <sys/defs.h>
//#include <sys/string.h> 
#include <sys/kprintf.h>
#include<sys/phymem.h>
#include<sys/virmem.h>
uint64_t *freelist;
uint64_t num_pages;
int count=0;
void mark_free(uint64_t i)
{
	freelist[i/64]&=~(1ull << (i%64));
}

void free_page(uint64_t page_addr)
{
	uint64_t index=page_addr/4096;
	mark_free(index);
}
void mark_used(uint64_t i)
{
	freelist[i/64]|=(1ull << (i%64));
}
int get_first_page()
{
	int i;
	count++;
	for(i=0;i<num_pages;i++)
	{
		uint64_t entry=i/64;
		int check_bit=i%64;
		uint64_t res=(1ull << check_bit);
		if(!(freelist[entry] & res))
			return entry*64 + check_bit;
	}
	return -1;
}
void* allocate_page()
{
	int i=get_first_page();
//	kprintf("    %d    ",i);
	mark_used(i);
	uint64_t addr=i*4096;
	return (void *)addr;
	
}
void make_free_list(uint64_t physbase,uint64_t physfree,uint64_t start,uint64_t end)
{
   freelist = (uint64_t *)physfree;
   int i=0;
   num_pages=(end/4096);
   //kprintf("%d\n",num_pages);
   while(i!=num_pages)
   {
	 mark_free(i);
	i++;
   } 
   for(i=0;i<=physfree;i+=4096)
     mark_used(i/4096);
   //kprintf("%d\n",i/4096);
    //kprintf("%s",kernmem);
}







