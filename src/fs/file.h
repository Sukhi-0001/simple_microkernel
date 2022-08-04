#ifndef FILE_H
#define FILE_H
#define MAX_FILESYSTEMS 12
#define MAX_FILE_DESCRIPTORS 512
#define MAX_PATH_NAME 108
#include "../disk/disk.h"
#include "./path_parser.h"

enum FILE_MODE{ 
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

enum FILE_SEEK_MODE
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

enum FILE_STAT_FLAGS
{
    FILE_STAT_READ_ONLY = 0b00000001
};

struct file_stat
{
    enum FILE_STAT_FLAGS flags;
    uint32_t filesize;
};


struct disk;
struct path_part;

struct filesystem{
    void*(*open_function)(struct disk* disk, struct path_part* path,enum FILE_MODE mode);
    int (*resolve_function)(struct disk* disk);
    int (*read_function)(struct disk* disk,void* private, uint32_t size, uint32_t nmemb, char* out);
    int (*seek_function)(void* private, uint32_t offset, enum FILE_SEEK_MODE seek_mode);
    int (*stat_function)(struct disk* disk,void* private,struct file_stat* stat);

    char name[20];
};

struct file_descriptor
{
    // The descriptor index
    int index;
    struct filesystem* filesystem;

    // Private data for internal file descriptor
    void* private;

    // The disk that the file descriptor should be used on
    struct disk* disk;
};

void fs_init();
void fs_insert_filesystem(struct filesystem* filesystem);
struct filesystem* fs_resolve(struct disk* disk);
int fopen(const char* filename,const char* mode);
int fstat(int fd, struct file_stat* stat);
int fseek(int fd, int offset, enum FILE_SEEK_MODE whence);
int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd);

#endif