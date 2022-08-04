#ifndef STRING_H
#define STRING_H

int strlen(const char* ptr);
int strnlen(const char* ptr,int max);
int is_digit(char a);
int to_numeric_digit(char a);
int strcmp(char *a,char *b,int len);
int istrncmp(char *a,char *b,int n);

char* strcpy(char* dest, const char* src);

char* strncpy(char* dest, const char* src, int count);
#endif