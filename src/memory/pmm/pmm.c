#include "pmm.h"
#include "../../kernel.h"
#include <stdint.h> 

//bitmap of memory
uint32_t pmm_memory_map[PMM_MAX_BLOCKS];
// used blocks of memory
uint32_t pmm_used_blocks=0;
//it tells free blocks omemory in system
uint32_t pmm_free_blocks=0;
// it is used for maxmum memory available to system 4096 bytes
// 1=4096 bytes
// 2=8k bytes
// it stores how many blocks are available to system
uint32_t pmm_system_memory_size=0; 


int pmm_get_block_count(){
    return pmm_system_memory_size;
}

int pmm_get_free_block_count(){
    return pmm_free_blocks;
}

void pmm_memory_map_set(uint32_t bit){
    if(bit==0){
        pmm_memory_map[0] |= (1<<(0));
        return;
    }
    pmm_memory_map[bit/32] |= (1<<(bit % 32));
}

void pmm_memory_map_unset(uint32_t bit){
    if(bit==0){
        pmm_memory_map[0] &= ~(1<<(0));
        return;
    }
    pmm_memory_map[bit/32] &= ~(1<<(bit % 32));
}

uint8_t pmm_memory_map_test(uint32_t bit){
    if(bit==0){
         if(pmm_memory_map[0] & (1<<(0)))
            return 1;
        return 0;
    }    

     if(pmm_memory_map[bit/32] & (1<<(bit % 32)))
        return 1;
    return 0;
}

int pmm_get_first_free_block(){
    for(uint32_t i=0;i<pmm_get_block_count();i++){
       // printf("%i \n",i);
        if(!pmm_memory_map_test(i))
            return i;
        
    }
    return -1;
}

void* pmm_allocate_block(){
    if(pmm_free_blocks<=0)   //no memory free blocks
        return 0;

    int frame=pmm_get_first_free_block();

    if(frame<=0) //out of memory // i think is might be not needed // just for precaution
    return 0; 

    pmm_memory_map_set(frame);

    pmm_free_blocks--;
    pmm_used_blocks++;

    //calculate free block staring physical address
    uint8_t* physical_addr=(uint8_t*)(0);
    physical_addr+=frame*PMM_BLOCK_SIZE;
    return (void *)physical_addr; 
}

// workneed to done
// always get pass starting block address aligined to 4096 bytes
void pmm_free_block(void *ptr){
    uint32_t temp=(uint32_t)ptr;
    pmm_free_blocks++;
    pmm_used_blocks--;
    pmm_memory_map_unset(temp/PMM_BLOCK_SIZE);
}

//work needed to be done
// this function is used to initilization of pysical memory
// 1 argument is size of memory available to system to used in 4k block
// 2 argument is kernel should tell how much physical memory it is going to reserve for it self 
void pmm_init(uint32_t system_memory_size,uint32_t memory_reserved_for_kernel){
   // pmm_memory_map=0x00200000;
    for (uint32_t i = 0; i < PMM_MAX_BLOCKS; i++)
    {
        pmm_memory_map_unset(i);
    }
    pmm_system_memory_size=system_memory_size;
    pmm_used_blocks=memory_reserved_for_kernel;
    pmm_free_blocks=system_memory_size-pmm_used_blocks;
    for(uint32_t i=0;i<memory_reserved_for_kernel;i++){
        pmm_memory_map_set(i);
    }
}

uint32_t pmm_check_range(uint32_t start,uint32_t end){
    for (uint32_t i = start; i < end; i++)
    {
        if(pmm_memory_map_test(i)==1)
            return 1;
    }
    return 0;
}

uint32_t pmm_get_first_n_blocks(uint32_t n){
    uint32_t start_block=1;
    uint32_t end_block=start_block+n;
    for(start_block=1;end_block<pmm_system_memory_size;start_block++){
        if(pmm_check_range(start_block,end_block)==0)
            return start_block;
        end_block++;
    }
    return 0;
}

void* pmm_allocate_first_n_blocks(uint32_t n){
    uint32_t start_block=pmm_get_first_n_blocks(n);
    for(int i=start_block;i<start_block+n;i++){
        pmm_memory_map_set(i);
    }

    if(start_block<=0)
        return 0;

    pmm_used_blocks+=n;
    pmm_free_blocks-=n;

    

    return ((void*)(start_block*PMM_BLOCK_SIZE)) ;
}