#include "string.h"
#include <stdint.h>
//tested
//
int strlen(const char* ptr){
    int count=0;
    while (*ptr!=0)
    {
        count++;
        ptr++;
    }
    return count;
}

//tested
int strnlen(const char* ptr,int max){
    int i;
    for (i = 0; i < max; i++)
    {
        if((*ptr)==0)
            break;
        ptr++;
    }
    return i;
}

//tested
int is_digit(char a){
    return a>=48 && a<=57;
}

//tested
int to_numeric_digit(char a){
    return a-48;
}

//tested
//please pass string which you already know equal in length
int strcmp(char *a,char *b,int len){
    int i;
    for ( i = 0; i < len; i++)
    {
        if(*a != *b)
        return 0;
        a++;
        b++;
    }
    return 1;
}

char to_upper(char a){
    if(a<97 || a>127)
        return a;
    return a-32;
}

int istrncmp(char *a,char *b,int n){
    while (n)
    {
        if(*a!=*b && to_upper(*a)!=to_upper(*b))
                return (*a)-(*b);
        a++;
        b++;
        n--;
    }
    return 0;
}

char* strcpy(char* dest, const char* src)
{
    char* res = dest;
    while(*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return res;
}

char* strncpy(char* dest, const char* src, int count)
{
    int i = 0;
    for (i = 0; i < count-1; i++)
    {
        if (src[i] == 0x00)
            break;

        dest[i] = src[i];
    }

    dest[i] = 0x00;
    return dest;
}