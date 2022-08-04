#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>

void* memset(void* ptr, int c, uint32_t size);
int memcmp(void* s1,void* s2,uint32_t count);
void* memcpy(void* des,void* src,uint32_t count);

#endif