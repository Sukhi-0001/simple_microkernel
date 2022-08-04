#include "file.h"
#include "../kernel.h"
#include "../string/string.h"
#include "./fat16/fat16.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"

struct filesystem *filesystems[MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[MAX_FILE_DESCRIPTORS];


int fs_get_free_slot(){
    int i=0;
    for (i = 1; i < MAX_FILESYSTEMS ; i++)
    {
        if(filesystems[i]==0)
            return i;
    }
    return -1;
}

void fs_insert_filesystem(struct filesystem* fs){
    int slot=fs_get_free_slot();
    if(slot<0){
        panic("NO FREE SLOT FILESYSTEM AVAILABLE");
    }
    filesystems[slot]=fs;
}

void fs_static_load(){
   fs_insert_filesystem( fat16_init());
}

void fs_load(){
    memset(filesystems,0,MAX_FILESYSTEMS);
    fs_static_load();
}

void fs_init(){
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd < 1 || fd >=MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    // Descriptors start at 1
    int index = fd;
    return file_descriptors[index];
}


// pass pointer of file des it will make it  point to fle dec
int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -1;
    for (int i = 1; i < MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));
            // Descriptors start at 1
            desc->index = i ;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}


struct filesystem* fs_resolve(struct disk* disk)
{
    struct filesystem* fs = 0;
    for (int i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve_function(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

enum FILE_MODE file_mode_get_by_string(char* str){
    if(istrncmp(str,"r",1)==0)
        return  FILE_MODE_READ;
    if(!istrncmp(str,"w",1)==0)
        return FILE_MODE_WRITE;
    if(!istrncmp(str,"a",1)==0)
        return FILE_MODE_APPEND;
    return FILE_MODE_INVALID;
}

//bug
//undefiend behaviour if wrong path is passed
int fopen(const char* filename,const char* mode){
    struct path_root* root_path=Pathparser_parse((char**)&filename);
    if(!root_path)
        return -1;

    if(!root_path->first)
        return -1;

    struct disk* disk=disk_get(root_path->drive_path);

    if(!disk)
        return -1;
    
    if(!disk->filesystem)
        return -1;
    
    enum FILE_MODE f_mode=file_mode_get_by_string((char*)mode);
    if(f_mode==FILE_MODE_INVALID)
        return -1;
    void *des_private_data=disk->filesystem->open_function(disk,root_path->first,f_mode);
    if(!des_private_data)
        return -1;
    struct file_descriptor* des;
    if(file_new_descriptor(&des)!=0)
        return -1;
    des->filesystem=disk->filesystem;
    des->private=des_private_data;
    des->disk=disk;

    if(des->index<0)
        return -1;

    return des->index;
}

int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd)
{
    int res = 0;
    if (size == 0 || nmemb == 0 || fd < 1)
    {   
       
        return -1;
    }
     
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        return -1;
    }

    
    res = desc->filesystem->read_function(desc->disk, desc->private, size, nmemb, (char*) ptr);
    
    return res;
} 

int fseek(int fd, int offset, enum FILE_SEEK_MODE whence)
{
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        return -1;
    }

    int res = desc->filesystem->seek_function(desc->private, offset, whence);
    return res;
}


int fstat(int fd, struct file_stat* stat)
{
    int res = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -1;
        goto out;
    }

    res = desc->filesystem->stat_function(desc->disk, desc->private, stat);
out:
    return res;
}
