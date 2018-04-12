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
void * updatedrsp;
int pid[2048];
struct task *current;
struct task *next;
struct task *init_kernel;
struct task *zombie_list=NULL;
extern uint64_t vir_start;
extern char _binary_tarfs_start;
extern char _binary_tarfs_end;
uint64_t stack_offset;
uint64_t heap_start=0x44440000;
uint64_t heap_end=0x66660000;
uint64_t stack_start=0x777770000;
uint64_t stack_end=0x777770000+0x10000;
extern FILE VFS[100];
extern uint64_t ticks;
uint64_t current_processes;
//extern parent_rsp;


//                current->fd_table[i].address =(uint64_t *)VFS[i].f_address;
void copy_fdtable(struct task * current1)
{
    int i=0;
    for(i=0;i<20;i++)
    {
        current1->fd_table[i].f_address = VFS[i].f_address;
        strcpy(VFS[i].name, current1->fd_table[i].name);
       	 current1->fd_table[i].fd=VFS[i].fd;
        current1->fd_table[i].size=VFS[i].size;
        current1->fd_table[i].parent=VFS[i].parent;
        //strcpy(VFS[i].flag, current->fd_table[i].flag);
        current1->fd_table[i].reference_count=VFS[i].referenceCount;
    }
}
int getPid()
{
	int i;
	for(i=0;i<2048;i++)
	{
		if(pid[i]==0)
		{
			pid[i]=1;
			current_processes=i;
			return i;
		}
	}
	return -1;
}
void switch_to()
{	
	__asm volatile("movq %%rdi, %0":"=r"(current->rdi));
	__asm volatile("movq %%rax, %0":"=r"(current->rax));
	__asm volatile("movq %%rsp, %0":"=r"(current->rsp));
	__asm volatile("movq %0, %%rsp":: "r"(next->rsp));
	__asm volatile("movq %0, %%rdi":: "r"(next->rdi));
	__asm volatile("movq %0, %%rax":: "r"(next->rax));
}
uint64_t wait()
{
		current->state=4;
		current->kern_rsp=parent_rsp;
		struct task * child=current->next_task;
		current->childpid=child->pid;
		child->state=RUNNING;
		//struct task *prev=current;
		set_tss_rsp((void *)(child->kern_rsp));
		current=child;
		uint64_t start_addr=((uint64_t)current->cr3 & 0x000000000fffffful);
		__asm volatile("movq %0, %%cr3":: "b"(start_addr));
		__asm volatile("movq %0, %%rsp":: "b"(current->kern_rsp));
		__asm volatile("movq %0, %%rax":: "r"((uint64_t)0));
		__asm volatile("iretq");
	return 0;
}
uint64_t yield()
{
		current->kern_rsp=parent_rsp;
                struct task * child=current->next_task;
                current->childpid=child->pid;
                set_tss_rsp((void *)(child->kern_rsp));
                current=child;
                uint64_t start_addr=((uint64_t)current->cr3 & 0x000000000fffffful);
                __asm volatile("movq %0, %%cr3":: "b"(start_addr));
                __asm volatile("movq %0, %%rsp":: "b"(current->kern_rsp));
                __asm volatile("movq %0, %%rax":: "r"((uint64_t)0));
                __asm volatile("iretq");
return 0;	
}
uint64_t  ps()
{
char *status;
status=(char *)mmap(100);
kprintf("ProcessName:%s Process %d  Status:%s   \n","Init-Kernel",0, "READY");
struct task * temp=init_kernel->next_task;
	while(temp->pid!=1)
	{
		if(temp->state==1)
                {
                        strcpy(status,"RUNNING");
                }
                if(temp->state==4)
                {
                       strcpy(status,"READY");
                }
//	memcpy((void *) buf, (void *) input_buffer, 1024);
                kprintf("ProcessName:%s  Id %d  Status:%s   \n",temp->name,temp->pid, status);	
		//kprintf("ProcessName:%s ProcessiD %d  Status:%d   ",temp->name,temp->pid, temp->state);
		temp=temp->next_task;	
	}
	if(temp->state==1)
        {
                strcpy(status,"RUNNING");
        }
        if(temp->state==4)
        {
         	strcpy(status,"READY");       
        }
	kprintf("ProcessName:%s   Id: %d   Status:%s   \n",temp->name,temp->pid, status);
	struct task *temp_zombies=zombie_list;
	while(temp_zombies!=NULL)
	{
		 strcpy(status,"KILLED");
		 kprintf("ProcessName:%s Id: %d  Status:%s   \n",temp_zombies->name,temp_zombies->pid, status);
		 temp_zombies=temp_zombies->next_task;
	}
return 0;	
}
void switch_to_ring3()
{ 	
       set_tss_rsp((void *)(next->kern_rsp));
	next->cr3=(uint64_t *)((uint64_t)next->cr3 & 0x000000000fffffful);
	__asm volatile("cli;"\
	"movq %0,%%cr3;"\
        "mov $0x23,%%ax;"\
        "mov %%ax,%%ds;"\
        "mov %%ax,%%es;"\
        "mov %%ax,%%fs;"\
        "mov %%ax,%%gs;"\
	"movq %1,%%rax;"\
        "pushq $0x23;"\
        "pushq %%rax;"\
        "pushfq;"\
	"popq %%rax;"\
        "orq $0x200,%%rax;"\
        "pushq %%rax;"\
        "pushq $0x2B;"\
        "pushq %2;"\
	"movq $0x0,%%rdi;"\
	"movq $0x0,%%rsi;"
        ::"r"(next->cr3),"r"(next->rsp),"r"(next->rip));
	__asm volatile("iretq;");
}

uint64_t sleep(uint64_t time)
{
//__asm("sti;");
uint64_t curr_ticks=0;
while(curr_ticks!=1193180*time*5)
{
	curr_ticks++;
}
return 0;
}




uint64_t execve(uint64_t argv,uint64_t argc)
{
	int len=strlen((char *)argv);
	int arg_len=strlen((char *)argc);
	char file_name[len+1];
	char args[arg_len+1];
	memcpy(file_name,(char *)argv,len);
	file_name[len]='\0';
	memcpy(args,(char *)argc,arg_len);
	args[arg_len]='\0';
	int i=0;
	while(file_name[i]!='\0')
	{
		current->name[i]=file_name[i];
		i++;
	}
	struct vma * temp=current->mm_struct;
	while(temp!=NULL)
	{
			struct vma *delete=temp;
			temp=temp->next;
			delete->next=NULL;
			free_phy_page((uint64_t)delete);	
	}
	clear_page_tables();
	uint64_t *pml4=(uint64_t *)get_pml4();
	uint64_t cr3=((uint64_t)pml4 & 0x000000000fffffful);
        __asm volatile("movq %0, %%cr3":: "b"(cr3));	
	current->mm_struct=NULL;
	exec_binary(file_name,current);
//allocate space for stack
	uint64_t p_addr=(uint64_t)allocate_page();	
	map_vir_phy_user(stack_end-0x1000,p_addr,pml4);
	struct vma *new = (struct vma *)kmalloc();
	new->start_addr=(uint64_t)stack_start;
	new->end_addr=stack_end;
	new->next=NULL;
	new->type=2;
	temp = current->mm_struct;
	while(temp->next!=NULL)
		temp=temp->next;
	temp->next=new;	
	current->heap=heap_start;
	uint64_t addr = mmap(100);
	memcpy((char *)addr,args,arg_len);
	current->user_stack=(char *)stack_end-0x1000;
	current->rsp=(uint64_t)current->user_stack + 0x1000 - 16;
	//current->rsp= (uint64_t)current->user_stack + 0x1000 - 8;
	*((uint64_t *)(current->rsp))=addr;
	current->rsp=current->rsp -8;
	switch_to_ring3();
return 1;
}

uint64_t mmap(uint64_t length)
{
	uint64_t val=-1;
	struct vma *new=(struct vma *)kmalloc();
	new->type=3;
	if(current->mm_struct ==NULL)
	{	
		new->start_addr=(uint64_t)current->heap;
		new->end_addr=(uint64_t)current->heap + 0x1000;
		current->heap=current->heap+0x1000;
		new->next=NULL;
		return new->start_addr;
	}
	else
	{
		struct vma *temp=current->mm_struct;
		while(temp!=NULL && temp->next!=NULL)
		{
			if(temp->start_addr <= heap_start || temp->start_addr>=heap_end)
				temp=temp->next;
		        else if((temp->next->start_addr) -(temp->end_addr) > length)			 {
					new->start_addr=temp->end_addr+1;
					new->end_addr=new->start_addr +length;
					new->next=temp->next;
					temp->next=new;
					return new->start_addr;		
		        }
		}
		temp->next=new;
		new->start_addr=current->heap;
		new->end_addr=current->heap +length;
		current->heap=current->heap+length;
		//kprintf("\n %p", new->start_addr);
		val =  (uint64_t)new->start_addr;
                //kprintf("\n val : %d", val);
		return val;
	}
    return val;
}
void move_to_zombie(struct task *zombie)
{
	zombie->next_task=NULL;
	struct task * temp=zombie_list;
	if(temp==NULL)
	{
		zombie_list=zombie;
		return;
	}
	while(temp->next_task!=NULL)
	{
		temp=temp->next_task;
	}
	temp->next_task=zombie;
	
}
uint64_t is_waiting(int pid)
{
	struct task *temp=init_kernel;
	while(temp->pid!=pid)
	{
		temp=temp->next_task;
	}
	if(temp->state==2 || temp->state==4)
        {
             temp->state=1;
             return 1;
        }
	return 0;
}
uint64_t exit(uint64_t status)
{
	current->state=3;
	clear_child_memory();
	struct task * temp=current->next_task;
	struct task * temp1=init_kernel;
	while(temp1->next_task!=current)
	{
		temp1=temp1->next_task;
	}
	temp1->next_task=current->next_task;
	if(is_waiting(current->ppid))
		free_phy_page((uint64_t)current);
	else
		move_to_zombie(current);
	//free_phy_page((uint64_t)current);
	struct vma * temp_vma=current->mm_struct;
	       while(temp_vma!=NULL)
	       {
				struct vma *delete=temp_vma;
				temp_vma=temp_vma->next;
				delete->next=NULL;
				free_phy_page((uint64_t)delete);
	       }

	current=temp;
	temp->state=RUNNING;
	uint64_t start_addr=((uint64_t)temp->cr3 & 0x000000000fffffful);
	set_tss_rsp((void *)(temp->kern_rsp));
	__asm volatile("movq %0, %%cr3":: "b"(start_addr));
	__asm volatile("movq %0, %%rsp":: "b"(temp->kern_rsp));
	__asm volatile("iretq;");
	//__asm volatile("movq %0, %%rax":: "r"((uint64_t)0));
	return 0;	
}
uint64_t kill(uint64_t pid)
{
	if(pid >= (current->pid-1))
	{
		kprintf("process does not exist\n");
		return 0;
	}
	struct task *temp2=zombie_list;
	while(temp2!=NULL)
	{
		if(temp2->pid==pid)
		{
			kprintf("process already killed");
			return 0;
		}
	temp2=temp2->next_task;
	}
	struct task * temp1=init_kernel;
	while(temp1->next_task->pid!=pid)
	{
		temp1=temp1->next_task;
	}
	struct task * delete=temp1->next_task;
	if(delete->pid==1)
	{
		temp1->next_task = init_kernel;
		current->ppid=0;
	}
	temp1->next_task=delete->next_task;
	free_phy_page((uint64_t)delete);
	return 0;
}
uint64_t fork()
{
	struct task *child=(struct task *)kmalloc();
 	child->pid=getPid();
	child->next_task=NULL;
	uint64_t * pml4c = (uint64_t *)kmalloc();
	child->cr3=pml4c;
	child->next_task=NULL;
	child->ppid=current->pid;
	child->heap=heap_start;
	current->next_task=child;
	struct task *temp=init_kernel;
	while(temp->next_task!=current)
	{
		temp=temp->next_task;
	}
	temp->next_task=child;
	child->next_task=current;
	struct vma * head=current->mm_struct;
	int i;
	for(i=0;i<20;i++)
	{
		child->fd_table[i]=current->fd_table[i];
	}
//	struct vma * child_vma=NULL;
	while(head!=NULL)
	{
		struct vma * new=(struct vma *)kmalloc();
		memcpy(new,head,sizeof(struct vma));
		new->next=NULL;
		head=head->next;
		if(child->mm_struct==NULL)
		{
			child->mm_struct=new;
		}
		else
		{
			struct vma *temp=child->mm_struct;
			while(temp->next!=NULL)
			{
				temp=temp->next;
			}
			temp->next=new;
		}
		//copy_parent_pt(next->cr3,child->cr3);		
	}
	uint64_t parent_pml4=(uint64_t) current->cr3 | vir_start;
	copy_parent_pt((uint64_t *)parent_pml4,child->cr3);
	uint64_t * par_pml4=(uint64_t *)parent_pml4;
	child->cr3[511]=par_pml4[511];
	child->kernstack=(char *)kmalloc();
	child->kern_rsp=(uint64_t)child->kernstack + 0x1000 - 8;
//	uint64_t *addr=(uint64_t *)allocate_page();
//	map_vir_phy_user((uint64_t)stack_start,(uint64_t)addr,child->cr3);
	child->user_stack=(char *)stack_start;
	child->rsp=(uint64_t)stack_start + 0x1000 - 8;
//	__asm volatile("movq %%rsp, %0": "=r"(parent_sp));
	stack_offset=current->kern_rsp-parent_rsp;
	uint64_t start_addr=((uint64_t)child->cr3 & 0x000000000fffffful);
 	__asm volatile("movq %0, %%cr3":: "b"(start_addr));
	//memcpy((void *)child->rsp-0x1000 +8,(void*)next->rsp-0x1000 +8,4088);
	memcpy((void *)child->kern_rsp-0x1000 +8,(void*)current->kern_rsp-0x1000 +8,4088);
	start_addr=((uint64_t)current->cr3 & 0x000000000fffffful);
        //__asm volatile("movq %0, %%cr3":: "b"(current->cr3));
        __asm volatile("movq %0, %%cr3":: "b"(start_addr));
	//__asm volatile("movq %0, %%cr3":: "b"(current->cr3));
	child->kern_rsp=child->kern_rsp-stack_offset;
	return child->pid;
}

//the iniital kernel process 
void initial_kernel_thread()
{
	struct task *task1=(struct task *)kmalloc();
	task1->state=RUNNING;
	//task1->pid=getPid();
	task1->kernstack=(char *)kmalloc()+0x1000;
	task1->rsp=(uint64_t)task1->kernstack-8;
	task1->cr3=(uint64_t *)((uint64_t)get_cr3() | vir_start);
	*(uint64_t *)(task1->rsp)=(uint64_t)(&thread1_func);

	struct task *task3=(struct task *)kmalloc();
	task3->state=SLEEPING;
	task3->pid=getPid();
	current=task1;
	uint64_t * pml4u = (uint64_t *)kmalloc();
	task3->cr3=pml4u;
	uint64_t curr_cr3 = (uint64_t)get_cr3();
	uint64_t *c3_vir = (uint64_t *)(curr_cr3 | vir_start);
	task3->cr3[511] = c3_vir[511];
	uint64_t *addr=(uint64_t *)allocate_page();
	map_vir_phy_user((uint64_t)stack_end-0x1000,(uint64_t)addr,pml4u);
	task3->user_stack=(char *)stack_end-0x1000;
	task3->rsp=(uint64_t)task3->user_stack + 0x1000 - 8;
	struct vma *new = (struct vma *)kmalloc();
	new->start_addr=(uint64_t)stack_start;
	new->end_addr=stack_end;
	new->next=NULL;
	new->type=2;
	task3->mm_struct=new;
	task3->kernstack=(char *)kmalloc();
	task3->ppid=0;
	task3->kern_rsp=(uint64_t)(task3->kernstack) + 0x1000 -16;
	task3->heap=heap_start;
	init_kernel=current;
	uint64_t start_addr=((uint64_t)task3->cr3 & 0x000000000fffffful);
	 __asm volatile("movq %0, %%cr3":: "b"(start_addr));
	copy_fdtable(task3);
	next=task3;
	current=next;
	exec_binary("bin/init",task3);
	start_addr=((uint64_t)task1->cr3 & 0x000000000fffffful);
	__asm volatile("movq %0, %%cr3":: "b"(start_addr));
	 __asm volatile("mov %%rsp,%0":"=a"(init_kernel->kern_rsp)::"memory");
	init_kernel->next_task=current;
	current->next_task=init_kernel;
	switch_to_ring3();
}
