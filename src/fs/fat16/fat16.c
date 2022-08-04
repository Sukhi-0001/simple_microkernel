#include "fat16.h"
#include "../../disk/disk.h"
#include "../path_parser.h"
#include "../file.h"
#include "../../memory/memory.h"
#include "../../kernel.h"
#include "../file.h"
#include "../../string/string.h"

struct filesystem fat16_fs =
    {
        .resolve_function = fat16_resolve,
        .open_function = fat16_open,
        .read_function = fat16_read,
        
        .seek_function = fat16_seek,
        .stat_function = fat16_stat,
        /*
        .close = fat16_close
        */
    };

struct filesystem *fat16_init()
{
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}



void fat16_init_private(struct disk* disk, struct fat16_private* private)
{
    memset(private, 0, sizeof(struct fat16_private));
    private->cluster_read_stream = create_disk_streamer(disk->id);
    private->fat_read_stream = create_disk_streamer(disk->id);
    private->directory_stream = create_disk_streamer(disk->id);
}


void fat16_free_directory(struct fat16_directory* directory)
{
    if (!directory)
    {
        return;
    }

    if (directory->item)
    {
        kfree(directory->item);
    }

    kfree(directory);
}


void fat16_fat_item_free(struct fat_item* item)
{
    if (item->FAT_ITEM_TYPE == FAT_ITEM_TYPE_DIRECTORY)
    {
        fat16_free_directory(item->directory);
    }
    else if(item->FAT_ITEM_TYPE == FAT_ITEM_TYPE_FILE)
    {
        kfree(item->item);
    }

    kfree(item);
}


uint32_t fat16_sectors_to_abosulute(struct disk* disk ,uint32_t sector){
    return disk->sector_size*sector;
}

int fat16_get_total_items_for_directory(struct disk* disk , uint32_t start_sector){

    struct fat16_private* fat16_private=disk->fs_private;
    struct fat16_directory_item current_directory_item;
    memset(&current_directory_item,0,sizeof(current_directory_item));
    
    //make disk stream to read directory
    struct disk_stream *directory_stream=fat16_private->directory_stream;
    disk_streamer_seek(directory_stream,fat16_sectors_to_abosulute(disk,start_sector));

    int i=0;
        while(1)
    {
        if (disk_streamer_read(directory_stream, &current_directory_item, sizeof(current_directory_item))<=0 )
        {
            return -1;
        }

        if (current_directory_item.filename[0] == 0x00)
        {
            // We are done
            break;
        }

        // Is the item unused
        if (current_directory_item.filename[0] == 0xE5)
        {
            continue;
        }

        i++;
    }

    return i;
}

int fat16_load_root_directory(struct disk* disk,struct fat16_private* fat16_private,struct fat16_directory* root_directory){
   // int res=0;
    struct fat16_header_primary* fat16_primary_header=&fat16_private->header.primary_header;

    // calculating no of sectors root directory is spread
    uint32_t root_dir_size=(fat16_primary_header->root_dir_entries*sizeof(struct fat16_directory_item));
    int root_sectors=root_dir_size/fat16_primary_header->bytes_per_sector;
    
    root_sectors=root_dir_size%fat16_primary_header->bytes_per_sector==0 ? root_sectors : root_sectors+1;
  
    // calculating starting sector for root directory
    int reserved_sectors=fat16_primary_header->hidden_setors+fat16_primary_header->reserved_sectors;
    int fat16_sectors_size=fat16_primary_header->fat_copies*fat16_primary_header->sectors_per_fat;             
    int root_directory_start_sector=reserved_sectors+fat16_sectors_size;

    //allocating space for all root directory
    fat16_private->root_directory.item=kzalloc(root_sectors*fat16_primary_header->bytes_per_sector);

    

    // creating stream for root directory
    struct disk_stream* root_dir_streamer=fat16_private->directory_stream;

    //set position to stream root dir
    disk_streamer_seek(root_dir_streamer,fat16_sectors_to_abosulute(disk,root_directory_start_sector));

    // read root directory in memory at fat16_private->root_directory
    if(disk_streamer_read(root_dir_streamer,root_directory->item,root_sectors*512)<=0)
        return -1;
    
    root_directory->start_sector_pos=root_directory_start_sector;
    int total_items=fat16_get_total_items_for_directory(disk,root_directory_start_sector);

    root_directory->start_sector_pos=root_directory_start_sector;
    
    root_directory->ending_sector_pos=root_directory_start_sector+root_sectors;
    root_directory->total_entries=total_items;

    return 1;
    
}

/*
it called by VFS layer to bind filesystem to disk
if it return 0 fat16 filesystem exits on disk
and this function binds itself to filesystem pointer on disk struct
*/
int fat16_resolve(struct disk* disk){
    
    int res=0;
    // allocate space for fat16_private 
    //like root dir and streams to read
    struct fat16_private* fat16_private=kzalloc(sizeof(struct fat16_private));

    //this function init all parameters to fat16
    fat16_init_private(disk,fat16_private);

    //create stream to load 1 block
    struct disk_stream* head_streamer=create_disk_streamer(disk->id);
    if(!head_streamer)
        return -1;
    
    //streamer not able to read
    if(disk_streamer_read(head_streamer,&fat16_private->header,sizeof(fat16_private->header))<=0){
        return -1; 
    }

    // if fat16 signaute not present it means it is not fat16 filesystem
    if(fat16_private->header.extended_header.signature!=0x29)
        return -1;

    //fat16 filesystem present till here
    //disk->filesystem
    //bind fat16_private to disk private
    disk->fs_private=fat16_private;

    //load root_directory in fat16_private->root_directory
    if(fat16_load_root_directory(disk,fat16_private,&(fat16_private->root_directory))!=1){
        return -1;
    }

    if(head_streamer)
        disk_streamer_close(head_streamer);
    return res;
}

void fat16_to_proper_string(char **out,char* in){
    
    while(*in != 0x00 && *in != 0x20)
    {
        **out = *in;
        *out += 1;
        in +=1;
    }

    if (*in == 0x20)
    {
        **out = 0x00;
    }
    
}


void fat16_get_relative_path_name(struct fat16_directory_item* item,char* out,int max_len){
    //set output buffer to null
    memset(out,0,max_len);
    char* temp_out=out;
    fat16_to_proper_string(&temp_out,(char*)item->filename);

    if (item->ext[0] != 0x00 && item->ext[0] != 0x20)
    {
        *temp_out++ = '.';
        fat16_to_proper_string(&temp_out,(char*)item->ext);
    }
}

int fat16_cluster_to_sector(struct fat16_private* fat16_private,int cluster){
    
    return fat16_private->root_directory.ending_sector_pos+((cluster-2)*fat16_private->header.primary_header.sectors_per_cluster);
}


 uint32_t fat16_get_first_cluster(struct fat16_directory_item* item)
{
    return (item->high_16_bits_first_cluster) | item->low_16_bits_first_cluster;
};

uint32_t fat16_get_first_fat_sector(struct fat16_private* private)
{
    return private->header.primary_header.reserved_sectors+private->header.primary_header.hidden_setors;
}

//this function gives entry for cluster in fat_table
uint32_t fat16_get_fat_entry(struct disk* disk,int cluster){
    int res = -1;
    struct fat16_private *private = disk->fs_private;
    struct disk_stream *stream = private->fat_read_stream;
    uint32_t fat16_table_start_in_bytes=fat16_get_first_fat_sector(private)*private->header.primary_header.bytes_per_sector;
    uint16_t result;
    uint32_t fat16_table_entry_offset_in_bytes=fat16_table_start_in_bytes+(cluster*FAT16_FAT_TABLE_ENTRY_SIZE);
    if(disk_streamer_seek(stream,fat16_table_entry_offset_in_bytes)<0)
        return res;
    res=disk_streamer_read(stream,&result,sizeof(result));
    if(res<0)
        return -1;
 
    return result;
}

static int fat16_get_cluster_for_offset(struct disk* disk, int starting_cluster, int offset)
{
    int res = 0;
    struct fat16_private* private = disk->fs_private;
    int size_of_cluster_in_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = starting_cluster;
    int clusters_ahead = offset / size_of_cluster_in_bytes;
    for (int i = 0; i < clusters_ahead; i++)
    {
        int entry = fat16_get_fat_entry(disk, cluster_to_use);
        if (entry == 0xFF8 || entry == 0xFFF)
        {
            // We are at the last entry in the file
            res = -1;
            goto out;
        }

        // Sector is marked as bad?
        if (entry == FAT16_BAD_SECTOR)
        {
            res = -1;
            goto out;
        }

        // Reserved sector?
        if (entry == 0xFF0 || entry == 0xFF6)
        {
            res = -1;
            goto out;
        }

        if (entry == 0x00)
        {
            res = -1;
            goto out;
        }

        cluster_to_use = entry;
    }

    res = cluster_to_use;
out:
    return res;
}


int fat16_read_internal_from_stream(struct disk* disk, struct disk_stream* stream, int cluster, int offset, int total, void* out){
    struct fat16_private* fs_private=disk->fs_private;
    int size_of_cluster_bytes = fs_private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);
    if (cluster_to_use < 0)
    {
        return -1;
    }

    int offset_from_cluster = offset % size_of_cluster_bytes;
    int starting_sector = fat16_cluster_to_sector(fs_private, cluster_to_use);

    int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
    int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
   
    if(disk_streamer_seek(stream,starting_pos)<0)
        return -1;
    
    if(disk_streamer_read(stream,out,total)<0)
        return -1;

    total-=total_to_read;
    if (total > 0)
    {
        // We still have more to read
        return fat16_read_internal_from_stream(disk, stream, cluster, offset+total_to_read, total, out + total_to_read);
    }
    // kernel_print("in fat16 read internal stream");
return 0;
}

int fat16_read_internal(struct disk* disk, int starting_cluster, int offset, int total, void* out){

    struct fat16_private* fs_private = disk->fs_private;
    struct disk_stream* stream = fs_private->cluster_read_stream;
    return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

//directory can also span mutiple clusters
//we should load all data of directory in memory
//it loads fat16 directory in memory
struct fat16_directory* fat16_load_directory(struct disk* disk,struct fat16_directory_item* dir_item){

    struct fat16_directory* directory = 0;
    struct fat16_private* fat_private = disk->fs_private;
    if (!(dir_item->attribute & FAT_FILE_SUBDIRECTORY))
    {
        return 0;
    }

    directory = kzalloc(sizeof(struct fat16_directory));

    if (!directory)
    {
        return 0; 
    }
    
    int cluster = fat16_get_first_cluster(dir_item);
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total_entries=total_items;
    int directory_size = directory->total_entries * sizeof(struct fat16_directory_item);
    directory->item = kzalloc(directory_size);
    
    if(directory->item==0)
        return 0;

    if(fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item)<0){
        fat16_free_directory(directory);
        return 0;
    }

    return directory;
}

struct fat16_directory_item* fat16_clone_directory_item(struct fat16_directory_item* item, int size)
{
    struct fat16_directory_item* item_copy = 0;
    if (size < sizeof(struct fat16_directory_item))
    {
        return 0;
    }

    item_copy = kzalloc(size);
    if (!item_copy)
    {
        return 0;
    }

    memcpy(item_copy, item, size);
    return item_copy;
}

/*
it return fat_item
it set fat_item type=1 for file 
it set fat_item type=0 for directory
*/
struct fat_item* fat16_new_fat_item_directory_item(struct disk *disk,struct fat16_directory_item *item){
    //allocate memory of fat item to return
    struct fat_item* f_item=kzalloc(sizeof(struct fat_item));
    if(!f_item)
        return 0;
    //if item is fat directory
    if(item->attribute & FAT_FILE_SUBDIRECTORY){
        f_item->FAT_ITEM_TYPE=FAT_ITEM_TYPE_DIRECTORY;
        f_item->directory=fat16_load_directory(disk,item);
        return f_item;
    }
    f_item->FAT_ITEM_TYPE=FAT_ITEM_TYPE_FILE;
    f_item->item=fat16_clone_directory_item(item,sizeof(struct fat16_directory_item));
    return f_item;

}


// it finds file presnet in directory pass to it 
struct fat_item* fat16_find_item_in_directory(struct disk* disk,struct fat16_directory* directory,const char* name){
    char tmp_filename[MAX_PATH_NAME];
    struct fat_item* f_item = 0;
     for(int i=0;i<directory->total_entries;i++){
         //it converts filename store in directory to char with exstention
        fat16_get_relative_path_name(directory->item+i,tmp_filename,MAX_PATH_NAME);
    
      
        if(istrncmp(tmp_filename,(char*)name,strlen(name))==0){
            f_item=fat16_new_fat_item_directory_item(disk,directory->item+i);
        }

     }

     return f_item;
}

struct fat_item* fat16_get_directory_entry(struct disk* disk,struct path_part* path_part){
    struct fat16_private* fat_private = disk->fs_private;
    struct fat_item* current_item = 0;
    struct fat_item* root_item 
            = fat16_find_item_in_directory(disk, &fat_private->root_directory, path_part->part);
    if (!root_item)
    {
        goto out;
    }

    struct path_part* next_part = path_part->next;
    current_item = root_item;
    while(next_part != 0)
    {
        if (current_item->FAT_ITEM_TYPE != FAT_ITEM_TYPE_DIRECTORY)
        {
            current_item = 0;
            break;
        }

        struct fat_item* tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
        fat16_fat_item_free(current_item);
        current_item = tmp_item;
        next_part = next_part->next;
    }
out:
    return current_item;


return 0;
}

void* fat16_open(struct disk* disk,struct path_part* path_part,enum FILE_MODE mode){
    if(mode!=FILE_MODE_READ){
        return 0;
    }

    // create a desriptor which will identify a file or directory
    struct fat_file_descriptor* descriptor = 0;
    descriptor = kzalloc(sizeof(struct fat_file_descriptor));
    if (!descriptor)
    {
        return 0;
    }

     //char buffer[20];

    struct fat_item* f_item =fat16_get_directory_entry(disk,path_part);
    if(!f_item)
        kernel_print("file not found");

    descriptor->item = fat16_get_directory_entry(disk, path_part);
    if (!descriptor->item)
    {
        return 0;
    }

    descriptor->pos = 0;
    return descriptor;

   // fat16_read_internal(disk,fat16_get_first_cluster(f_item->item),0,20,buffer);
    return 0;
}


int fat16_read(struct disk* disk, void* descriptor, uint32_t size, uint32_t nmemb, char* out_ptr)
{
    struct fat_file_descriptor* fat_desc = descriptor;
    struct fat16_directory_item* item = fat_desc->item->item;
    int offset = fat_desc->pos;
    for (uint32_t i = 0; i < nmemb; i++)
    {
        int res = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out_ptr);
        if (res<0)
        {   
            kernel_print("in if fat16 read");
            kernel_print_uint(res);
            return -1;
        }

        out_ptr += size;
        offset += size;
    }

    return nmemb;
}

int fat16_seek(void *private, uint32_t offset,enum FILE_SEEK_MODE seek_mode)
{
    int res = 0;
    struct fat_file_descriptor *desc = private;
    struct fat_item *desc_item = desc->item;
    if (desc_item->FAT_ITEM_TYPE != FAT_ITEM_TYPE_FILE)
    {
        res = -1;
        goto out;
    }

    struct fat16_directory_item *ritem = desc_item->item;
    if (offset >= ritem->filesize)
    {
        res = -1;
        goto out;
    }

    switch (seek_mode)
    {
    case SEEK_SET:
        desc->pos = offset;
        break;

    case SEEK_END:
        res = -1;
        break;

    case SEEK_CUR:
        desc->pos += offset;
        break;

    default:
        res = -1;
        break;
    }
out:
    return res;
} 

int fat16_stat(struct disk* disk, void* private, struct file_stat* stat)
{
    int res = 1;
    struct fat_file_descriptor* descriptor = (struct fat_file_descriptor*) private;
    struct fat_item* desc_item = descriptor->item;
    if (desc_item->FAT_ITEM_TYPE != FAT_ITEM_TYPE_FILE)
    {
        res = -1;
        goto out;
    }

    struct fat16_directory_item* ritem = desc_item->item;
    stat->filesize = ritem->filesize;
    stat->flags = 0x00;

    if (ritem->attribute & FAT_FILE_READ_ONLY)
    {
        stat->flags |= FILE_STAT_READ_ONLY;
    }
out:
    return res;
}
