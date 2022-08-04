#include "streamer.h"
#include "disk.h"
#include "../kernel.h"
#include "../memory/heap/kheap.h"

struct disk_stream* create_disk_streamer(int disk_id){
    struct disk* disk=disk_get(disk_id);
    if(!disk)
        return 0;
    struct disk_stream* streamer=kmallac(sizeof(struct disk_stream));
    streamer->pos=0;
    streamer->disk=disk;
    return streamer;
}

int disk_streamer_seek(struct disk_stream* streamer,int pos){
    streamer->pos=pos;
    return 0;
}

int disk_streamer_read(struct disk_stream* stream,void * out ,uint32_t total){
    uint32_t sector;
    if(stream->pos==0)
        sector=0;
    else
        sector=stream->pos/512;
    uint32_t offset=stream->pos%512;
    char buffer[512];
    
    int res=disk_read_block(stream->disk,sector,1,buffer);

    if(res<0)
        return res;
    
    
    uint32_t total_to_read= total>512 ? 512 : total;
    for (uint32_t i = 0; i < total_to_read; i++)
    {
        *((char*)out)=buffer[offset+i];
        (char*)out++;
    }

    // if stream size bigger than 1 sector

    stream->pos+=total_to_read;
    if(total>512)
        res+=disk_streamer_read(stream,out,total-512);
    return res;

}

void disk_streamer_close(struct disk_stream* streamer){
    kfree(streamer);
}
