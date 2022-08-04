#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>

#define PTE_PRESENT             0b00000001
#define PTE_WRITEABLE           0b00000010
#define PTE_ACCESS_FROM_ALL     0b00000100
#define PTE_wRITE_THROUGH       0b00001000
#define PTE_CACHE_DISABLED      0b00010000

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096


struct paging_4gb_chunk
{
    uint32_t* directory_entry;
};

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
void paging_switch(uint32_t* directory);
void enable_paging();
uint32_t* paging_get_current_directory();

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);
void paging_free_4gb(struct paging_4gb_chunk* chunk);
int paging_map_to(uint32_t *directory,void *virtual_addr,void *phy_addr,void *phy_addr_end,int flags);
int paging_map_range(uint32_t *directory,void *virtual_addr,void *phy_addr,uint32_t total_pages,int flags);
int paging_map(uint32_t *directory,void *virtual_addr,void* phy_addr,int flags);

void* paging_align_address(void* ptr);
uint32_t paging_get(uint32_t* directory, void* virt);
int paging_set(uint32_t* directory, void* virt, uint32_t val);


#endif