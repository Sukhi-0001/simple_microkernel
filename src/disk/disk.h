#ifndef DISK_H
#define DISK_H
#include <stdint.h>
#include "../fs/file.h"

#define DISK_TYPE_REAL 0;

struct disk
{
    unsigned int disk_type;
    unsigned int sector_size;

    int id;

    struct filesystem* filesystem;

    void *fs_private;
};

void disk_search_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf);
#endif