#include "kheap.h"
#include "../pmm/pmm.h"
#include "../memory.h"
#include "../../kernel.h"
#define BIG_ADDRESS 100

struct big_address{
    void* starting_address;
    uint32_t blocks;
};

struct big_address big_blocks[BIG_ADDRESS];

void big_blocks_init(){
    for(uint32_t i=0;i<BIG_ADDRESS;i++){
        big_blocks[i].starting_address=0;
        big_blocks[i].blocks=0;
    }
}

int insert_in_big_blocks(void* addr,uint32_t blocks){
    for(int i=0;i<BIG_ADDRESS;i++){
        if(big_blocks[i].starting_address==0){
            big_blocks[i].blocks=blocks;
            big_blocks[i].starting_address=addr;
            break;
        }
    }
    return -1;
}

int search_address(void* item_addr){
    for(int i=0;i<BIG_ADDRESS;i++){
        if(big_blocks[i].starting_address==item_addr)
            return i;
    }
    return -1;
}

void free_big_blocks(void* addr){
    int index=search_address(addr);
    if(index==-1)
        return;
    
    for(int i=0;i<big_blocks[index].blocks;i++){
        uint32_t current_addr=(uint32_t)addr+(i*PMM_BLOCK_SIZE);
        pmm_free_block((void*)current_addr);
    }

    big_blocks[index].starting_address=0;
    big_blocks[index].blocks=0;
}



void *kmallac(size_t size){
    //to allocate for size big than 4096
    if(size>4096){
        int blocks_to_allocate=size/4096;
        blocks_to_allocate= size%4096==0 ? blocks_to_allocate : blocks_to_allocate+1; 
        void* addr=pmm_allocate_first_n_blocks(blocks_to_allocate);
        if(addr<=0)
            return 0;
    insert_in_big_blocks(addr,blocks_to_allocate);
    return addr;
    }
    return pmm_allocate_block();
}
void kfree(void* addr){
    

    uint32_t temp=(uint32_t)addr;
    if(temp%4096!=0)
        return;

    if(search_address(addr)>=0){
       // kernel_print("free big addr");
        free_big_blocks(addr);
        return;
    }

    pmm_free_block(addr);
}

void* kzalloc(size_t size){
    void* temp=kmallac(size);
    memset(temp,0,size);
    return temp;
}

