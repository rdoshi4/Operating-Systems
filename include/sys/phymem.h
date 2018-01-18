#ifndef __PHYMEM_H
#define __PHYMEM_H

 void make_free_list(uint64_t physbase,uint64_t physfree,uint64_t start,uint64_t end);
 void* allocate_page();
 void free_page(uint64_t page_addr);
#endif

