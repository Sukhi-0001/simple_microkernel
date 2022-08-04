#include "path_parser.h"
#include "../string/string.h"
#include "../memory/heap/kheap.h"
#include "../memory/memory.h"
#include "../kernel.h"

//tested
//it checks only first 3
int pathparser_path_valid_format(char *str){
    int len=strnlen(str,MAX_PATH);

    if(len<=3)
        return 0;
    if(is_digit(str[0])!=1)
        return 0;
    if(memcmp(&str[1],":/",2)!=0)
        return 0;
    return 1;
}

int pathparser_get_drive_by_path(char **path){
    if(pathparser_path_valid_format(*path)==0)
        return -1;
    int drive_no=to_numeric_digit((*path)[0]);
    *path=*path+3;
    return drive_no;
}

struct path_root* pathparser_create_path_root(char **path){
    struct path_root *temp=(struct path_root*)kzalloc(sizeof(struct path_root));
    temp->drive_path=pathparser_get_drive_by_path(path);
    temp->first=0;
    return temp;
}

struct path_part *pathparser_get_path_part(char **path){
    struct path_part *temp=(struct path_part*)kzalloc(sizeof(struct path_part)); // kernel heap
    char *c_temp=(char*)kzalloc(MAX_PATH); //kernel heap
    int i=0;
    while (**path != '/' && **path!=0)
    {
        c_temp[i]=**path;
        (*path)++;
        i++;
    }   
    c_temp[i]='\0';

    if(**path=='/')
        (*path)++;
    
    if(i==0){
        kfree(temp);
        return 0;
    }
    temp->part=c_temp;
    return temp;
}

struct path_part* pathparser_create_path_part(char** path)
{
   // struct path_part* current_path_part;//=(struct path_part*)malloc(sizeof(struct path_part)); //kernel heap should be here
    struct path_part *current_path_part=pathparser_get_path_part(path);
    struct path_part *temp=current_path_part;

    while (current_path_part!=0)
    {
        current_path_part->next=pathparser_get_path_part(path);
        current_path_part=current_path_part->next;
    }
    
    return temp;
}

struct path_root* Pathparser_parse(char **path)
{
    struct path_root* current_path_root=pathparser_create_path_root(path);
    struct path_part* curernt_path_part=pathparser_create_path_part(path);
    current_path_root->first=curernt_path_part;
    return current_path_root;   
}
