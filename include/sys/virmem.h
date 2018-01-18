#ifndef __VIRMEM_H
#define __VIRMEM_H

#include<sys/initializeIDT.h>

//void setup_paging(uint64_t physfree);
void setup_paging(uint64_t physfree,uint64_t max_phyfree);
void set_cr3();
void *get_cr3();
void * kmalloc();
void map_vir_phy(uint64_t vaddr,uint64_t paddr);
void * user_malloc(uint64_t *pml4);
void map_vir_phy_user(uint64_t vaddr,uint64_t paddr,uint64_t *pml41);
void initializePageFaultHandler();
void copy_parent_pt(uint64_t *parent_pml4,uint64_t *child_pml4);
void page_fault_handler();
void free_phy_page(uint64_t addr);
void clear_page_tables(); 
#endif
