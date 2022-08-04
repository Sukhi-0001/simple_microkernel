#ifndef HEAP_H
#define HEAP_H
#include <stdint.h>
#include <stddef.h>

struct heap_header{
    size_t size;
    struct heap_header *next;
    int free;
}__attribute__((packed));

void* heap_init();
void* heap_mallock(size_t size_need);

#endif