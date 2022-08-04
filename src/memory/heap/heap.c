#include "heap.h"
#include "../pmm/pmm.h"
#include "./../../kernel.h"
#include "kheap.h"

struct heap_header* heap_head;


void *heap_init(){
    void big_blocks_init();
    /*
    heap_head=pmm_allocate_block();
    heap_head->size=PMM_BLOCK_SIZE-12;
    heap_head->next=0;
    heap_head->free=1;
    //kernel_print_uint((uint32_t) heap_head->size);
    return 0;
    */
   return 0;
}

//in future to implement a better heap maneger
/*

//tested
uint32_t get_aligin_4_bytes(uint32_t size_need){
    if(size_need%4!=0)
        return size_need+=4-(size_need%4);
    return size_need;
}

// this function only return point to header of free block
struct heap_header* heap_get_first_free_block(uint32_t size_need){
    struct heap_header* current=heap_head;
    
    while (current!=0 && current->free==0 && current->size<size_need+12 )// if current block on satisfy request
    {
        current=current->next;
    }

    if(current==0) //no free block of memory
        return 0;

    return current;  
}

void* heap_mallock(size_t size_need){
    if(size_need==0)
        return 0;
    size_need=get_aligin_4_bytes(size_need);
    struct heap_header *current_heap_header=heap_get_first_free_block(size_need);
    size_t phy_addr=0;
    phy_addr+=12+size_need+(uint32_t)current_heap_header;

    struct heap_header * new_heap_header=(struct heap_header*)phy_addr;

    kernel_print_uint((uint32_t)current_heap_header);
    kernel_print_uint((uint32_t)new_heap_header);

   // (struct heap_header*)new_heap_header=new_heap_header;
    new_heap_header->free=0;
    new_heap_header->size=current_heap_header->size-size_need-12;
    new_heap_header->next=current_heap_header->next;
    current_heap_header->next=new_heap_header;
    
    uint32_t temp=((uint32_t)(uint8_t*)current_heap_header+12);
    return (void *)temp;
}

*/