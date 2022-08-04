#include <stdint.h>
#include "paging.h"
#include "../pmm/pmm.h"
#include "../../kernel.h"

int for_one_time=1;

void paging_load_directory(uint32_t* directory);

static uint32_t* current_directory = 0;

uint32_t* paging_get_current_directory(){
    return current_directory;
}

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags)
{   
    
    uint32_t* directory = pmm_allocate_block();
    if(!directory){
        
        return 0;
    }
    int offset = 0;
    
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        uint32_t* entry = pmm_allocate_block();
        if(!entry){
            
            return 0;
        }
        
        for (int b = 0; b < PAGING_TOTAL_ENTRIES_PER_TABLE; b++)
        { 
            entry[b] = (offset + (b * PAGING_PAGE_SIZE)) | flags;
        }
        
        offset += (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE);
        directory[i] = (uint32_t)entry | flags | PTE_WRITEABLE;
    }
   
    
    struct paging_4gb_chunk* chunk_4gb = pmm_allocate_block();
    chunk_4gb->directory_entry = directory;
    for_one_time--;
    return chunk_4gb;
}

void paging_switch(uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk)
{
    return chunk->directory_entry;
}

uint32_t paging_is_aligned(void* addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
} 

int paging_get_indexes(void* virtual_address, uint32_t* directory_index_out, uint32_t* table_index_out)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
    {
        res = -1;
        goto out;
    }  

    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t) virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res;
}

uint32_t paging_get(uint32_t* directory, void* virt)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_indexes(virt, &directory_index, &table_index);
    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);
    return table[table_index];
}

int paging_set(uint32_t* directory, void* virt, uint32_t val)
{
    if (!paging_is_aligned(virt))
    {
        return -1;
    }

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    int res = paging_get_indexes(virt, &directory_index, &table_index);
    if (res < 0)
    {
        return res;
    }

    uint32_t entry = directory[directory_index];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);
    table[table_index] = val;
    return 1;
}


// this free all page table in memory
void paging_free_4gb(struct paging_4gb_chunk* chunk)
{
    for (int i = 0; i < 1024; i++)
    {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xfffff000);
        pmm_free_block(table);
    }

    pmm_free_block(chunk->directory_entry);
    pmm_free_block(chunk);
}

void* paging_align_address(void* ptr)
{
    if ((uint32_t)ptr % PAGING_PAGE_SIZE)
    {
        return (void*)((uint32_t)ptr + PAGING_PAGE_SIZE - ((uint32_t)ptr % PAGING_PAGE_SIZE));
    }
    
    return ptr;
}


int paging_map(uint32_t *directory,void *virtual_addr,void* phy_addr,int flags){
    if((uint32_t)virtual_addr%PAGING_PAGE_SIZE || (uint32_t)phy_addr%PAGING_PAGE_SIZE)
        return -1;
    return paging_set(directory,virtual_addr,(uint32_t)phy_addr | flags);
}

int paging_map_range(uint32_t *directory,void *virtual_addr,void *phy_addr,uint32_t total_pages,int flags){
    int res=0;
    for (uint32_t i = 0; i < total_pages; i++)
    {
        res=paging_map(directory,virtual_addr,phy_addr,flags);
        if(res<0)
            return -1;
        virtual_addr+=PAGING_PAGE_SIZE;
        phy_addr+=PAGING_PAGE_SIZE;
    }
    return 1;
}

int paging_map_to(uint32_t *directory,void *virtual_addr,void *phy_addr,void *phy_addr_end,int flags){
    int res=0;
    if ((uint32_t)virtual_addr % PAGING_PAGE_SIZE)
    {
        res = -1;
        goto out;
    }
    if ((uint32_t)phy_addr % PAGING_PAGE_SIZE)
    {
        res = -1;
        goto out;
    }
    if ((uint32_t)phy_addr_end % PAGING_PAGE_SIZE)
    { 
        res = -1;
        goto out;
    }

    if ((uint32_t)phy_addr_end < (uint32_t)phy_addr)
    {
        res = -1;
        goto out;
    }

    uint32_t total_bytes = phy_addr_end - phy_addr;
    int total_pages = total_bytes / PAGING_PAGE_SIZE;
    res = paging_map_range(directory, virtual_addr, phy_addr, total_pages, flags);
out:
    return res;
}



