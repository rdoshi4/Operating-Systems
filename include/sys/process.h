#ifndef __PROCESS_H
#define __PROCESS_H
#include<sys/tarfs.h>
#define RUNNING 1
#define SLEEPING 2
#define ZOMBIE 3
#define READY 4
#include<sys/initializeIDT.h>
#include<sys/defs.h>

uint64_t parent_rsp;
struct fd_entry
{
        uint64_t *address;
	uint64_t f_address;
        uint64_t reference_count;
	uint64_t parent;
	char flags[1];
	char name[100];
	int fd;
	uint64_t size;
};
struct task
{
 char * kernstack;
 char * user_stack;
 uint64_t pid;
 uint64_t rsp;
 uint64_t kern_rsp;
 uint64_t state;
 int exit_status; 
 uint64_t *cr3;
 uint64_t rdi;
 uint64_t rsi;
 uint64_t rax;
 uint64_t rip;
 uint64_t rbx;
 uint64_t rbp;
 struct vma *mm_struct;
 struct fd_entry fd_table[20];
 struct task * next_task;
 uint64_t ppid;
 uint64_t heap;
 uint64_t childpid;
 char name[20];
};
/*typedef struct{
  char name[100];
  int fd;
  uint64_t f_address;
  char typeflag[1];
  uint64_t size;
  uint64_t parent;
}FILE_proc;
*/
struct task *zombie_list;
struct vma
{
	uint64_t start_addr;
	uint64_t end_addr;
	struct vma *next;
	uint64_t flags;
	uint64_t ext;
	uint64_t type;
	//FILE *fp;
};
void initial_kernel_thread();
uint64_t fork();
//void mmap(uint64_t length,int flags);
//void *mmap(reg r);
//void mmap_handle(reg *r);
//void initializeSysCall();
uint64_t  mmap(uint64_t length);
uint64_t  wait();
void copy_fdtable(struct task * current);
uint64_t execve(uint64_t filename,uint64_t args);
void * get_pml4();
void clear_child_memory();
uint64_t exit(uint64_t status);
uint64_t sleep(uint64_t time);
uint64_t  ps();
uint64_t kill(uint64_t pid);
uint64_t yield();
#endif
