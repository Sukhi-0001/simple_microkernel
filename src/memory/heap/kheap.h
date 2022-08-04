#ifndef K_HEAP
#define K_HEAP
#include <stddef.h>

void *kmallac(size_t size);
void kfree(void* add);
void *kzalloc(size_t size);
void big_blocks_init();
#endif