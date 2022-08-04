#ifndef STREAMER_H
#define STREAMER_H

#include "disk.h"

struct disk_stream
{
    unsigned int pos;
    struct disk* disk;
};
int disk_streamer_read(struct disk_stream* stream,void * out ,uint32_t total);
int disk_streamer_seek(struct disk_stream* streamer,int pos);
struct disk_stream* create_disk_streamer(int disk_id);
void disk_streamer_close(struct disk_stream* streamer);

#endif