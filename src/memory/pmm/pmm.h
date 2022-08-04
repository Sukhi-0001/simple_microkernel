#ifndef pmm_h
#define pmm_h
#include <stdint.h>

//! 8 blocks per byte
#define PMM_BLOCKS_PER_BYTE 8
 
//! block size (4k)
#define PMM_BLOCK_SIZE	4096
 
//! block alignment
#define PMM_BLOCK_ALIGN	PMMNGR_BLOCK_SIZE

//  memory manger can refer only max 4gb of memory
// this much blocks
#define PMM_MAX_BLOCKS 32768

void pmm_init(uint32_t system_memory_size,uint32_t memory_reserved_for_kernel);
void* pmm_allocate_block();
void pmm_free_block(void *ptr);
void* pmm_allocate_first_n_blocks(uint32_t n);
uint8_t pmm_memory_map_test(uint32_t bit);
void pmm_memory_map_set(uint32_t bit);
void pmm_memory_map_unset(uint32_t bit);


#endif