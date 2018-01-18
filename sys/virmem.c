#include <sys/defs.h>
#include <string.h> 
#include <sys/kprintf.h>
#include<sys/virmem.h>
#include <sys/phymem.h>
#include <sys/gdt.h>
#include<sys/process.h>
extern char  kernmem,physbase;
uint64_t video_offset = 0xffffffff800b8000ul;
uint64_t start_addr;
//uint64_t start_offset=0xffffffff0000000ul;
 uint64_t vir_start=0xffffffff80000000ul;
uint64_t * pml4;
uint64_t * pdte;
uint64_t * pdt;
uint64_t * pt;
int enable_paging;
extern uint64_t * freelist;
extern struct task * current;
extern struct task * next;
extern uint64_t heap_start;
extern uint64_t heap_end;
extern uint64_t stack_start;
extern uint64_t stack_end;
void * get_pml4()
{
        __asm volatile("movq %%cr3, %0":: "b"(start_addr));
        return (void *)(start_addr |  0xffffffff80000000ul);
}
void clear_child_memory()
{
	int i,j,k,l;
        uint64_t  *pml4 = (uint64_t *)get_pml4();
        for(i=0;i<511;i++)
        {
                if(pml4[i]!=0x0)
                {
                        uint64_t pdte_entry=pml4[i];
                        pdte_entry=pdte_entry & 0xfffffffffffff000ull;
                        uint64_t *pdte= (uint64_t *)(pdte_entry | vir_start);
                        for(j=0;j<512;j++)
                        {
                                if(pdte[j]!=0x0)
                                {
                                                uint64_t pdt_entry=pdte[j];
                                                pdt_entry=pdt_entry & 0xfffffffffffff000ull;
                                                uint64_t *pdt= (uint64_t *)(pdt_entry | vir_start);
                                                for(k=0;k<512;k++)
                                                {
                                                        if(pdt[k]!=0x0)
                                                        {
                                                                uint64_t pt_entry=pdte[j];
                                                                pt_entry=pt_entry & 0xfffffffffffff000ull;
                                                                uint64_t *pt= (uint64_t *)(pt_entry | vir_start);
                                                                for(l=0;l<512;l++)
                                                                {
                                                                        if(pt[l]!=0x0)
                                                                        {
                                                                                if(pt[l] & (1<<9))
                                                                                        continue;
                                                                                else
                                                                                   {
                                                                                         uint64_t page=pt[l] & 0xfffffffffffff000ull;
                                                                                         free_page(page);
                                                                                         pt[l]=0x0;
											      
                                                                                   }
                                                                        }
									
                                                                }
							   free_page(pt_entry);
                                                        }
                                                }
					free_page(pdt_entry);	
				}
			}
			
			free_page(pdte_entry);				
	}
}	
}


void copy_parent_pt(uint64_t *parent_pml4,uint64_t *child_pml4)
{
	int i,k,l,m;
	for(i=0;i<511;i++)
	{
		if(parent_pml4[i]==0x0)
			child_pml4[i]=parent_pml4[i];
		else
		{
			
			//kprintf("%p",parent_pml4[i]);
			//parent_pml4[i]=parent_pml4[i] & 0xfffffffffffff000ull;
			//parent_pml4[i]=parent_pml4[i] | 0x5 | (1ull <<9); 
			uint64_t parent_pdte=parent_pml4[i] & 0xfffffffffffff000ull;
			parent_pdte= parent_pdte | 0xffffffff80000000ull;
			uint64_t *parent_pdte_ptr=(uint64_t *)parent_pdte;
			uint64_t * child_pdte;
			//child_pdte=(uint64_t *)kmalloc();
			child_pdte=(uint64_t *)allocate_page();
        		memset((void*)(0xffffffff80000000 | (uint64_t)child_pdte),0,4096);
        		child_pml4[i]=(uint64_t)child_pdte | 0x7;
			child_pdte=(uint64_t *)((uint64_t)child_pdte | vir_start);
			for(k=0;k<512;k++)
			{
				
				if(parent_pdte_ptr[k]==0x0)
					child_pdte[k]=parent_pdte_ptr[k];
				else
				{
						//child_pdte[k]=parent_pdte_ptr[k];
						//parent_pdte_ptr[k]=parent_pdte_ptr[k] & 0xfffffffffffff000ul;
                        			//parent_pdte_ptr[k]=parent_pdte_ptr[k] | 0x5 | (1ull <<9);
						uint64_t parent_pdt=parent_pdte_ptr[k] & 0xfffffffffffff000ul;
						parent_pdt=parent_pdt | 0xffffffff80000000ul;
						uint64_t * parent_pdt_ptr=(uint64_t *)parent_pdt;
						uint64_t *child_pdt;
						child_pdt=(uint64_t *)allocate_page();
                        			memset((void*)(0xffffffff80000000 | (uint64_t)child_pdt),0,4096);
                        			child_pdte[k]=(uint64_t)child_pdt | 0x7;
                       				child_pdt=(uint64_t*)((uint64_t)child_pdt | vir_start);
						for(l=0;l<512;l++)
						{
							if(parent_pdt_ptr[l]==0x0)
								child_pdt[l]=parent_pdt_ptr[l];
							else
							{
								//kprintf("%p",parent_pdt_ptr[i]);
								//child_pdt[l]=parent_pdt_ptr[l];
								//parent_pdt_ptr[l]=parent_pdt_ptr[l] & 0xfffffffffffff000ul;
                        					//parent_pdt_ptr[l]=parent_pdt_ptr[l] | 0x5 | (1ull <<9);
								uint64_t parent_pt=parent_pdt_ptr[l] & 0xfffffffffffff000ul;
								parent_pt=parent_pt | 0xffffffff80000000ul;
								uint64_t *parent_pt_ptr=(uint64_t *)parent_pt;
								uint64_t *child_pt;
								child_pt=(uint64_t *)allocate_page();
                        					memset((void*)(0xffffffff80000000 | (uint64_t)child_pt),0,4096);
                        					child_pdt[l]=(uint64_t)child_pt | 0x7;
                        					child_pt=(uint64_t *)((uint64_t)child_pt | vir_start);
								for(m=0;m<512;m++)
								{
										if(parent_pt_ptr[m]!=0x0)
										{
											parent_pt_ptr[m]=parent_pt_ptr[m] & 0xfffffffffffff000ul;
                        								parent_pt_ptr[m]=parent_pt_ptr[m] | 0x5 | (1ull <<9);
										//	kprintf(" %p ",parent_pt_ptr[m]);
										}
										child_pt[m]=parent_pt_ptr[m];
									//	kprintf(" %p ",child_pt[m]);
								}
							} 	
						}
				}
					
			}
		}
	}
}
/*void set_to_write()
{
	
	 int pml4_offset=(v_addr>>39) & 0x1ff;
        int pdte_offset=(v_addr>>30) & 0x1ff;
        int pdt_offset=(v_addr>>21)& 0x1ff;
        int pt_offset=(v_addr >>12)&0x1ff;
        uint64_t pdte_entry=pml4[pml4_offset];
        pdte_entry=pdte_entry & 0xfffffffffffff000ull;
        //pml4[pml4_offset] = pdte_entry | 0x7;
        uint64_t *pdte = (uint64_t *)(pdte_entry | vir_start);
        uint64_t pdt_entry = pdte[pdte_offset];
        pdt_entry=pdt_entry & 0xfffffffffffff000ull;
        //pdte[pdte_offset]=pdt_entry |0x7;
        uint64_t *pdt = (uint64_t *)(pdt_entry | vir_start);
        uint64_t pt_entry = pdt[pdt_offset];
        pt_entry=pt_entry & 0xfffffffffffff000ull;
        //pdt[pdt_offset]=pt_entry |0x7;
        uint64_t *pt = (uint64_t *)(pt_entry | vir_start);
        //if(pt[pt_offset] & (1 <<9))
	
}*/
uint64_t munmap(uint64_t v_addr)
{
	struct vma * temp=current->mm_struct;
	while(temp!=NULL)
	{
		if(temp->start_addr==v_addr)
		{
			uint64_t length = temp->end_addr - temp->start_addr;
			memset((void *)v_addr,0,length);
			uint64_t *pml4=(uint64_t *)get_pml4();
		        uint64_t start_addr=((uint64_t)pml4 & 0x000000000fffffful);
        		__asm volatile("movq %0, %%cr3":: "b"(start_addr));	
			return 0;
		}
		temp=temp->next;
	}
return -1;
	
}


int check_cow_bit(uint64_t v_addr,uint64_t *pml4)
{
        int pml4_offset=(v_addr>>39) & 0x1ff;
        int pdte_offset=(v_addr>>30) & 0x1ff;
        int pdt_offset=(v_addr>>21)& 0x1ff;
        int pt_offset=(v_addr >>12)&0x1ff;
        uint64_t pdte_entry=pml4[pml4_offset];
	if(pdte_entry!=0x0)
	{
        	pdte_entry=pdte_entry & 0xfffffffffffff000ull;
        //pml4[pml4_offset] = pdte_entry | 0x7;
       	 	uint64_t *pdte = (uint64_t *)(pdte_entry | vir_start);
        	uint64_t pdt_entry = pdte[pdte_offset];
		if(pdt_entry!=0x0)
		{
        		pdt_entry=pdt_entry & 0xfffffffffffff000ull;
        //pdte[pdte_offset]=pdt_entry |0x7;
        		uint64_t *pdt = (uint64_t *)(pdt_entry | vir_start);
        		uint64_t pt_entry = pdt[pdt_offset];
			if(pt_entry!=0x0)
			{
        			pt_entry=pt_entry & 0xfffffffffffff000ull;
        //pdt[pdt_offset]=pt_entry |0x7;
				uint64_t *pt = (uint64_t *)(pt_entry | vir_start);
        			if(pt[pt_offset] & (1 <<9))
					return 1;
				else 
					return 0;
			}
		}
	}
	return 0;
}

void free_phy_page(uint64_t addr)
{
	int pml4_offset=(addr>>39) & 0x1ff;
        int pdte_offset=(addr>>30) & 0x1ff;
        int pdt_offset=(addr>>21)& 0x1ff;
        int pt_offset=(addr >>12)&0x1ff;
	uint64_t *pml4=(uint64_t *)get_pml4();
	uint64_t pdte_entry=pml4[pml4_offset];

	if(pdte_entry!=0x0)
        {
                pdte_entry=pdte_entry & 0xfffffffffffff000ull;
        //pml4[pml4_offset] = pdte_entry | 0x7;
                uint64_t *pdte = (uint64_t *)(pdte_entry | vir_start);
                uint64_t pdt_entry = pdte[pdte_offset];
                if(pdt_entry!=0x0)
                {
                        pdt_entry=pdt_entry & 0xfffffffffffff000ull;
        //pdte[pdte_offset]=pdt_entry |0x7;
                        uint64_t *pdt = (uint64_t *)(pdt_entry | vir_start);
                        uint64_t pt_entry = pdt[pdt_offset];
                        if(pt_entry!=0x0)
                        {
                                pt_entry=pt_entry & 0xfffffffffffff000ull;
        //pdt[pdt_offset]=pt_entry |0x7;
                                uint64_t *pt = (uint64_t *)(pt_entry | vir_start);
                                if(pt[pt_offset] & (1 <<9))
                                        return;
                                else
				   {
                                       uint64_t page =pt[pt_offset] & 0xfffffffffffff000ull;
					//kprintf(" removed:%p ",page);
					free_page(page);
					//pt[pt_offset]=0x0;
					uint64_t start_addr=(uint64_t)pml4  & 0x000000000fffffful;
					__asm volatile("movq %0, %%cr3":: "b"(start_addr));
					//uint64_t new_page=(uint64_t)allocate_page();
					//kprintf("new: %p ",new_page);
					
				   }
                        }
                }
        }
}
void clear_page_tables()
{
	int i,j,k,l;
	uint64_t  *pml4 = (uint64_t *)get_pml4();
	for(i=0;i<511;i++)
	{
		if(pml4[i]!=0x0)
		{
			uint64_t pdte_entry=pml4[i];
			pdte_entry=pdte_entry & 0xfffffffffffff000ull;
			uint64_t *pdte= (uint64_t *)(pdte_entry | vir_start);
			for(j=0;j<512;j++)
			{
				if(pdte[j]!=0x0)
				{	
						uint64_t pdt_entry=pdte[j];
                        			pdt_entry=pdt_entry & 0xfffffffffffff000ull;
                        			uint64_t *pdt= (uint64_t *)(pdt_entry | vir_start);
						for(k=0;k<512;k++)
						{
							if(pdt[k]!=0x0)
							{
								uint64_t pt_entry=pdte[j];
                                                		pt_entry=pt_entry & 0xfffffffffffff000ull;
                                                		uint64_t *pt= (uint64_t *)(pt_entry | vir_start);
								for(l=0;l<512;l++)
								{
									if(pt[l]!=0x0)
									{
										if(pt[l] & (1<<9))
											continue;
										else
										   {
											 uint64_t page=pt[l] & 0xfffffffffffff000ull;
											 free_page(page);
											 pt[l]=0x0;	
										   }
									}
								}
							}
						}
				}
			}
		}
	}	
		
}
void free_vmas(struct vma * current_proc)
{
	if(current_proc==NULL)
		return;
	struct vma *temp=current_proc;
	while(temp!=NULL)
	{
		struct vma *clear=temp;
		temp=temp->next;
		free_page((uint64_t)clear);
	}
return;
	
}

/*uint64_t exit(uint64_t status)
{
struct task * temp;
	while(temp!=NULL)
	{
		
	}
	free_vmas(current->mm_struct);
//	delete_pt();
return 0;
}*/


uint64_t page_fault_handler_function(reg r)
{
	volatile uint64_t addr;
         __asm volatile("mov %%cr2,%0":"=r"(addr));
	//kprintf("\nInside page fault interrupt handler \n");
//	int pml4_offset=(addr>>39) & 0x1ff;
//	int pdte_offset=(addr>>30) & 0x1ff;
//	int pdt_offset=(addr>>21)& 0x1ff;
//	int pt_offset=(addr >>12)&0x1ff;
	uint64_t *pml4=(uint64_t *)get_pml4();
        //uint64_t p_addr=(uint64_t)allocate_page();
	//uint64_t v_addr=(addr & (0xfffffffffffffffull<<12));
	//map_vir_phy_user(v_addr,p_addr,pml4);
	//return 0;
	uint64_t cr3 = (uint64_t)pml4 & 0x000000000fffffful;
	int cow=check_cow_bit(addr,pml4);
	
	if(cow)
	{
              //  __asm volatile("mov %%cr2,%0":"=r"(addr));
        //uint64_t p_addr=(uint64_t)allocate_page();
                uint64_t v_addr=(addr & (0xfffffffffffffffull<<12));
                uint64_t *temp_buf =(uint64_t *)kmalloc();
                memcpy(temp_buf,(uint64_t *)v_addr,4096);
                uint64_t p_addr=(uint64_t)temp_buf & 0x000000000fffffful;
           //     set_to_write(v_addr,pml4);
                map_vir_phy_user(v_addr,p_addr,pml4);
		__asm volatile("movq %0, %%cr3":: "b"(cr3));
                return 1;
	}
	if(addr>=heap_start && addr<=heap_end)
	{
		uint64_t p_addr=(uint64_t)allocate_page();
		map_vir_phy_user(addr,p_addr,pml4);
		__asm volatile("movq %0, %%cr3":: "b"(cr3));
		return 1;
	}
	else if(addr>= stack_start && addr<=stack_end)
	{
				struct vma * temp;
				temp=next->mm_struct;	
				while(temp->type!=2)
				temp=temp->next;
				uint64_t p_addr=(uint64_t)allocate_page();
				map_vir_phy_user(addr,p_addr,pml4);
				__asm volatile("movq %0, %%cr3":: "b"(cr3));
				return 1;
	}
	else
	{
		kprintf("Segmentation fault");
		exit(1);
	}
return -1;
}
void initializePageFaultHandler()
{
//  page_fault_handler_function(r);
  handleRegInterrupt(14,page_fault_handler_function);
}

void set_cr3()
{
	__asm volatile("movq %0, %%cr3":: "b"(start_addr));
	pml4=(uint64_t *)((uint64_t)pml4 | 0xffffffff80000000ul);
}
void *get_cr3()
{
        __asm volatile("movq %%cr3, %0":: "b"(start_addr));
        return (void *)start_addr;
}

//map kernel virtual to physical
void map_vir_phy(uint64_t vaddr,uint64_t paddr)
{
int pml4_offset=(vaddr>>39) & 0x1ff;
int pdte_offset=(vaddr>>30) & 0x1ff;
int pdt_offset=(vaddr>>21)& 0x1ff;
int pt_offset=(vaddr >>12)&0x1ff;
uint64_t * pdte1;
uint64_t * pdt1;
uint64_t * pt1;
//kprintf(" %d %d %d1= %d",pt_offset,pdt_offset,pdte_offset,pml4_offset);
if(pml4[pml4_offset]!=0x0)
{
	pdte1=(uint64_t *)((pml4[pml4_offset]) & ~(0xffful));
//	kprintf(" %p ",pdte1);
	if(enable_paging)
	{
		//kprintf("%d %d %d %d\n", pml4_offset, pdte_offset, pdt_offset,pt_offset);
		uint64_t pdte1_vir=(uint64_t)pdte1 | 0xffffffff80000000;
		pdte1=(uint64_t *)pdte1_vir;
	}
}
else
{
	pdte1=(uint64_t *)allocate_page();
	memset((void*)(0xffffffff80000000 |(uint64_t)pdte1),0,4096);
	pml4[pml4_offset]=(uint64_t)pdte1 | 0x3;
	if(enable_paging)
        {
                uint64_t pdte1_vir=(uint64_t)pdte1 | 0xffffffff80000000;
                pdte1=(uint64_t *)pdte1_vir;
        }
	//kprintf("%p",pml4[pml4_offset]);
}
if(pdte1[pdte_offset]!=0x0)
{
        pdt1=(uint64_t *)((pdte1[pdte_offset]) & ~(0xffful));
        //kprintf(" %p ",pdt1);
	if(enable_paging)
        {
                uint64_t pdt1_vir=(uint64_t)pdt1 | 0xffffffff80000000;
                pdt1=(uint64_t *)pdt1_vir;
        }

}
else
{
        pdt1=(uint64_t *)allocate_page();
	memset((void*)(0xffffffff80000000 | (uint64_t)pdt1),0,4096);
        pdte1[pdte_offset]=(uint64_t)pdt1 | 0x3;
	if(enable_paging)
        {
                uint64_t pdt1_vir=(uint64_t)pdt1 | 0xffffffff80000000;
                pdt1=(uint64_t *)pdt1_vir;
        }

	//kprintf("%p",pdte1[pdte_offset]);
}
if(pdt1[pdt_offset]!=0x0)
{
        pt1=(uint64_t *)((pdt1[pdt_offset]) & ~(0xffful));
       // kprintf(" %p ",pt1);
	if(enable_paging)
        {
                uint64_t pt1_vir=(uint64_t)pt1 | 0xffffffff80000000;
                pt1=(uint64_t *)pt1_vir;
        }

}
else
{
        pt1=(uint64_t *)allocate_page();
	memset((void*)(0xffffffff80000000 | (uint64_t)pt1),0,4096);
        pdt1[pdt_offset]=(uint64_t)pt1 | 0x3;
	if(enable_paging)
        {
                uint64_t pt1_vir=(uint64_t)pt1 | 0xffffffff80000000;
                pt1=(uint64_t *)pt1_vir;
        }
	//kprintf("%p",pdt1[pdt_offset]);
}
pt1[pt_offset]=paddr | 0x3;
//kprintf("%p ",pt1[pt_offset]);
}


//identity mapping
void setup_virtual_paging(uint64_t max_phyfree)
{
	uint64_t addr=0;
	while(addr!=max_phyfree)
	{
		uint64_t addr_vir = 0xffffffff80000000 |addr;
		map_vir_phy(addr_vir,addr);
		addr+=4096;
	}
}

//kernel memory allocator
void * kmalloc()
{
	uint64_t addr = (uint64_t)allocate_page();
	//vir_start+=0x1000;
	//map_vir_phy(vir_start,addr);
	addr=addr|0xffffffff80000000ul;
	memset((void *)addr,0,4096);
	return (void *)(addr);
        
}


//map phy addresses to user space
void map_vir_phy_user(uint64_t vaddr,uint64_t paddr,uint64_t *pml41)
{
int pml4_offset=(vaddr>>39) & 0x1ff;
int pdte_offset=(vaddr>>30) & 0x1ff;
int pdt_offset=(vaddr>>21)& 0x1ff;
int pt_offset=(vaddr >>12)&0x1ff;

uint64_t * pdte1;
uint64_t * pdt1;
uint64_t * pt1;
//kprintf(" %d %d %d1= %d",pt_offset,pdt_offset,pdte_offset,pml4_offset);
if(pml41[pml4_offset]!=0x0)
{
        pdte1=(uint64_t *)((pml41[pml4_offset]) & ~(0xffful));
//      kprintf(" %p ",pdte1);
        if(enable_paging)
        {
                //kprintf("%d %d %d %d\n", pml4_offset, pdte_offset, pdt_offset,pt_offset);
                uint64_t pdte1_vir=(uint64_t)pdte1 | 0xffffffff80000000;
		//memset((void *)pdte1_vir,0,4096);
                pdte1=(uint64_t *)pdte1_vir;
        }
}
else
{
        pdte1=(uint64_t *)allocate_page();
	memset((void*)(0xffffffff80000000 | (uint64_t)pdte1),0,4096);
        pml41[pml4_offset]=(uint64_t)pdte1 | 0x7;
        if(enable_paging)
        {
                uint64_t pdte1_vir=(uint64_t)pdte1 | 0xffffffff80000000;
		//memset((void *)pdte1_vir,0,4096);
                pdte1=(uint64_t *)pdte1_vir;
        }
        //kprintf("%p",pml4[pml4_offset]);
}
if(pdte1[pdte_offset]!=0x0)
{
        pdt1=(uint64_t *)((pdte1[pdte_offset]) & ~(0xffful));
        //kprintf(" %p ",pdt1);
        if(enable_paging)
        {
                uint64_t pdt1_vir=(uint64_t)pdt1 | 0xffffffff80000000;
				//memset((void *)pdt1_vir,0,4096);
		                pdt1=(uint64_t *)pdt1_vir;
        }

}
else
{
        pdt1=(uint64_t *)allocate_page();
	memset((void*)(0xffffffff80000000 |(uint64_t)pdt1),0,4096);
        pdte1[pdte_offset]=(uint64_t)pdt1 | 0x7;
        if(enable_paging)
        {
                uint64_t pdt1_vir=(uint64_t)pdt1 | 0xffffffff80000000;
		//memset((void *)pdt1_vir,0,4096);
                pdt1=(uint64_t *)pdt1_vir;
        }

        //kprintf("%p",pdte1[pdte_offset]);
}
if(pdt1[pdt_offset]!=0x0)
{
        pt1=(uint64_t *)((pdt1[pdt_offset]) & ~(0xffful));
       // kprintf(" %p ",pt1);
        if(enable_paging)
        {
                uint64_t pt1_vir=(uint64_t)pt1 | 0xffffffff80000000;
		//memset((void *)pt1_vir,0,4096);
                pt1=(uint64_t *)pt1_vir;
        }

}
else
{
        pt1=(uint64_t *)allocate_page();
	memset((void*)(0xffffffff80000000 | (uint64_t)pt1),0,4096);
        pdt1[pdt_offset]=(uint64_t)pt1 | 0x7;
        if(enable_paging)
        {
                uint64_t pt1_vir=(uint64_t)pt1 | 0xffffffff80000000;
		//memset((void *)pt1_vir,0,4096);
                pt1=(uint64_t *)pt1_vir;
        }
        //kprintf("%p",pdt1[pdt_offset]);
}
pt1[pt_offset]=paddr | 0x7;	
}

/*void * user_malloc(uint64_t *pml41)
{
	uint64_t addr = (uint64_t)allocate_page();
	map_vir_phy_user(addr,addr,pml41);
	memset((void *)addr,0,4096);
	return (void *)addr;
}*/
//void * mmap()


//Initial paging 
void setup_paging(uint64_t physfree,uint64_t max_phyfree)
{
        //kprintf(" %p ",max_phyfree);
	pml4=(uint64_t *)allocate_page();
	start_addr=(uint64_t)pml4;
//	kprintf("%p",&kernmem);
	int i;
	for(i=0;i<512;i++)
	{
		pml4[i]=0x0;
	}
	pdte=(uint64_t *)allocate_page();
	for(i=0;i<512;i++)
        {
                pdte[i]=0x0;
        }
	pdt=(uint64_t *)allocate_page();
        for(i=0;i<512;i++)
        { 
		pdt[i]=0x0;
        }
        pt=(uint64_t *)allocate_page();
        for(i=0;i<512;i++)
        {
                pt[i]=0x0;
        }
	uint64_t kernlogical=(uint64_t)&kernmem;
	int pml4_offset=((kernlogical >> 39) & 0x1ff);
	int pdte_offset=((kernlogical >> 30) & 0x1ff);
	int pdt_offset=((kernlogical >> 21) & 0x1ff);
	int pt_offset=((kernlogical >> 12) & 0x1ff);
	//kprintf("%d %d %d %d",pml4_offset,pdte_offset,pdt_offset,pt_offset); 
	pml4[pml4_offset]=(uint64_t)pdte |0x3;
	pdte[pdte_offset]=(uint64_t)pdt |0x3;
	pdt[pdt_offset]=(uint64_t)pt |0x3;
	//pt[pt_offset]=(uint64_t)&physbase|0x1;
	uint64_t addr=(uint64_t)&physbase;
	while(addr!=0x240000)
	{
		//kprintf("%p",addr);
		pt[pt_offset++] = addr | 0x3;
		addr=addr+0x1000;
	}
	pdt_offset=(video_offset >> 21)  &0x1ff;
	pt_offset=(video_offset >> 12) & 0x1ff;
	uint64_t * pt1=(uint64_t *)allocate_page();
        pdt[pdt_offset]=(uint64_t)pt1 | 0x3;
	pt1[pt_offset]=(0xb8000) |0x3;
	uint64_t freelist_vir = ((uint64_t)freelist | 0xffffffff80000000);
	map_vir_phy(freelist_vir,(uint64_t)freelist);
	freelist=(uint64_t *)freelist_vir;
        uint64_t pml4_vir = ((uint64_t)pml4 | 0xffffffff80000000);
	map_vir_phy(pml4_vir,(uint64_t)pml4);
	pml4=(uint64_t *)pml4_vir;
	uint64_t pdte_vir = ((uint64_t)pdte | 0xffffffff80000000);
        map_vir_phy(pdte_vir,(uint64_t)pdte);
	pdte=(uint64_t *)pdte_vir;
        uint64_t pdt_vir = ((uint64_t)pdt | 0xffffffff80000000);
        map_vir_phy(pdt_vir,(uint64_t)pdt);
	pdt=(uint64_t *)pdt_vir;
        uint64_t pt_vir = ((uint64_t)pt | 0xffffffff80000000);
        map_vir_phy(pt_vir,(uint64_t)pt);
	pt=(uint64_t *)pt_vir;
	setup_virtual_paging(max_phyfree);
	//kprintf(" %d ",pt_offset);
}
