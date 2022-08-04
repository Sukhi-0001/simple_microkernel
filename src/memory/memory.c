#include <stdint.h>
#include "memory.h"

void* memset(void* ptr, int c, uint32_t size){  
    char *c_ptr=(char*)ptr;
    for(uint32_t i=0;i<size;i++){
        c_ptr[i]=(char)c;
    }
    return ptr;
}

int memcmp(void* s1,void* s2,uint32_t count){
    char *c_s1=(char *)s1;
    char *c_s2=(char *)s2;
    for (uint32_t i = 0; i < count; i++)
    {
        if(*c_s1 != *c_s2 )
            return *c_s1 < *c_s2 ? -1 : 1;
        c_s1++;
        c_s2++;
    }
    return 0;   
}

// now should be only used for equal
void* memcpy(void* des,void* src,uint32_t count){
    char* c_des=(char*)des;
    char* c_src=(char*)src;
    for (uint32_t i = 0; i < count; i++)
    {
        c_des[i]=c_src[i];
    }
    return des;
}