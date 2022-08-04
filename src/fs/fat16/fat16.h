#ifndef FAT16_H
#define FAT16_H
#include <stdint.h>
#include "../../disk/streamer.h"
#include "../path_parser.h"
#include "../../memory/heap/kheap.h"
#include "../../kernel.h"

#define FAT16_FAT_TABLE_ENTRY_SIZE 2 //it is in bytes
#define FAT16_BAD_SECTOR 0xFF7

#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1


// Fat directory entry attributes bitmask
#define FAT_FILE_READ_ONLY 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVED 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80

struct fat16_header_extended
{
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct fat16_header_primary
{
    uint8_t short_jmp_ins[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_setors;
    uint32_t sectors_big;
} __attribute__((packed));

struct fat16_header
{
    struct fat16_header_primary primary_header;
    struct fat16_header_extended extended_header;
}__attribute__((packed));

struct fat16_directory_item
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

struct fat16_directory
{
    struct fat16_directory_item *item;
    int total_entries;
    int start_sector_pos;
    int ending_sector_pos;
};

struct fat16_private
{
    struct fat16_header header;
    struct fat16_directory root_directory;

    // Used to stream data clusters
    struct disk_stream *cluster_read_stream;
    // Used to stream the file allocation table
    struct disk_stream *fat_read_stream;

    // Used in situations where we stream the directory
    struct disk_stream *directory_stream;
};

struct fat_item
{
    union {
        struct fat16_directory_item *item;
        struct fat16_directory *directory;
    };

    int FAT_ITEM_TYPE ;
};

struct fat_file_descriptor
{
    struct fat_item *item;
    uint32_t pos;
};


int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk,struct path_part* path_part,enum FILE_MODE mode);
int fat16_read(struct disk* disk, void* descriptor, uint32_t size, uint32_t nmemb, char* out_ptr);
void fat16_to_proper_string(char **out,char* in);
int fat16_stat(struct disk* disk, void* private, struct file_stat* stat);
int fat16_seek(void *private, uint32_t offset,enum FILE_SEEK_MODE seek_mode);

struct filesystem *fat16_init();
#endif