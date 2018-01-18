#include <sys/defs.h>
#include <string.h>
#include <sys/kprintf.h>
#include <sys/virmem.h>
#include <sys/phymem.h>
#include <sys/gdt.h>
#include <sys/process.h>
#include<sys/tarfs.h>
#include <sys/initializeIDT.h>
#include<sys/elf64.h>
extern FILE VFS[100];
void exec_binary(char *filename,struct task *current)
{
	int fd=open((uint64_t)filename);
	Elf64_Ehdr *e_hdr=(Elf64_Ehdr *)VFS[fd].f_address;
	//kprintf("e_phnum:%x", e_hdr->e_entry);
	
	Elf64_Phdr *p_hdr=(Elf64_Phdr *)((uint64_t)e_hdr+e_hdr->e_phoff);
	//kprintf("type: %x",phdr->p_type);
	int i;
	for(i=0;i<e_hdr->e_phnum;i++)
	{
		//kprintf("hi");
		struct vma *temp=(struct vma*)current->mm_struct;
		if(p_hdr->p_type==1)
		{	
			//kprintf("vaddr %x",p_hdr->p_vaddr);

			struct vma *new=(struct vma *)kmalloc();
			new->next=NULL;
			if(current->mm_struct==NULL)
			{
				current->mm_struct=new;
			}
			else
			{
			       //struct vma *temp=(struct vma*)0xffffffff80277000;
				while(temp->next!=NULL)
					temp=temp->next;
				temp->next=new;
				//temp=NULL;
			}
			new->flags=p_hdr->p_flags;
			//kprintf("%d",new->flags);
			new->start_addr=p_hdr->p_vaddr;
			new->end_addr=p_hdr->p_vaddr + p_hdr->p_memsz;
			uint64_t size=(((new->end_addr)/4096)*4096 + 4096)-(((new->start_addr)/4096)*4096);
			uint64_t pages=(size/4096)+1;
			uint64_t start=(new->start_addr/4096)*4096;
			while(pages)
			{
				uint64_t addr=(uint64_t)allocate_page();
				map_vir_phy_user(start,addr,current->cr3);
				pages--;
				start=start+4096;	
			}
		
			if(new->flags==(1|4)|| new->flags==(2|4))
			{	
				//kprintf("yes!!");			
		        	char *cdest =( char *)new->start_addr;
        	        	char *csrc =( char *)(e_hdr+p_hdr->p_offset);
                       		int j;
        			for(j=0;j<p_hdr->p_filesz;j++)
                			cdest[j]=csrc[j];
			}	
			//memcpy((void*)new->start_addr,(void *)e_hdr+p_hdr->p_offset,p_hdr->p_filesz);	
			p_hdr++;
			
		}
		
	}	
current->rip=e_hdr->e_entry;
}
