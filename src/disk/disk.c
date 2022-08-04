#include "disk.h"
#include "../kernel.h"
#include <stdint.h>
#include "../io/io.h"

struct disk disk;

/* this function read from disk using LBA 48bits 
it can read max around 2.7 tb disk where sector size is 512 bytes
*/
int disk_read_sectors(uint32_t lba,uint32_t total,void *buffer){
  /*
    char lba_digit[6];
    lba_digit[0]=lba;
    lba_digit[1]=lba>>8;
    lba_digit[2]=lba>>16;
    lba_digit[3]=lba>>24;
    lba_digit[4]=0;
    lba_digit[5]=0;
*/
    
    outb(0x01F6, 0xE0); //problm in this line
   
    outb(0x01F2, total); // sectors count registor
    outb(0x1F3, (unsigned char)(lba & 0xff));
     
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);
    

    unsigned short* ptr = (unsigned short*) buffer;
    for (int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = inb(0x1F7);
        while(!(c & 0x08))
        {
            c = inb(0x1F7);
        }

        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++)
        {
            *ptr = inw(0x1F0);
            ptr++;
        }

    }
    return 0;
}

void disk_search_init(){
    disk.disk_type=DISK_TYPE_REAL;
    disk.sector_size=512;
    disk.id=0;
    disk.filesystem=fs_resolve(&disk);
}

struct disk* disk_get(int index){
    if(index!=0)
        return 0;
    return &disk;
}

int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buf)
{
    if (idisk != &disk)
    {
        return -1;
    }

    return disk_read_sectors(lba, total, buf)==0 ?1:0;
}