#include "kernel.h"
#include "./io/io.h"
#include "./idt/idt.h"
#include "./memory/memory.h"
#include "./memory/pmm/pmm.h"
#include "./memory/paging/paging.h"
#include "./memory/heap/heap.h"
#include "./memory/heap/kheap.h"
#include "./disk/disk.h"
#include "./disk/streamer.h"
#include "./fs/path_parser.h"
#include "./string/string.h"
#include "./fs/file.h"
#include "./fs/fat16/fat16.h"
#include "./gdt/gdt.h"
#include "./task/tss.h"
#include "./task/task.h"
#include "./task/process.h"
#include "./isr80h/isr80h.h"
#include "./chips/pit/pit.h"
#include <stdint.h>
#include <stddef.h>

uint16_t *video_memory=0;
uint16_t terminal_row=0;
uint16_t terminal_col=0;
struct paging_4gb_chunk* kernel_chunk;

uint16_t terminal_make_char(char a,char colour){
    uint16_t temp=(uint16_t)colour;
    return (temp<<8)|a;
    }

void terminal_init(){
    terminal_row=0;
    terminal_col=0;
    video_memory=(uint16_t *)(0xB8000);
    for(int x=0;x<VGA_HEIGHT;x++){
        for(int y=0;y<VGA_WIDTH;y++){
            terminal_put_char(y,x,' ',0);
        }
    }
}

// col,row

void terminal_get_char(int y,int x){

}

void terminal_put_char(int y, int x, char c, char colour)
{
    video_memory[(x * VGA_WIDTH) + y] = terminal_make_char(c, colour);
}


// always print white char
void kernel_terminal_print_char(char a){
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
    if(terminal_row>=VGA_HEIGHT){
        terminal_init();
    }
    terminal_put_char(terminal_col,terminal_row,a,15);
    terminal_col++;
}

void terminal_scroll(){
    
    for (int i = 0; i < VGA_HEIGHT-1; i++)
    {
        for (int j = 0; i < VGA_WIDTH; j++)
        {
           
        }
        
    }
    
}

// add new line automatically
// can only print 20 times
// as no scrolling is enable
void kernel_print_string(char *str,uint32_t len){
    for(int i=0;i<len;i++){
        kernel_terminal_print_char(*str);
        str++;
    }
    terminal_row += 1;
    terminal_col = 0;
}

uint32_t kernel_str_lenght(char *a){
    uint32_t count=0;
    for(a=a;(*a)!='\0';a++){
        count++;
    }
    return count;
}

void kernel_print(char *str){
    kernel_print_string(str,kernel_str_lenght(str));
}

// tested till 9 digits in length print
void kernel_print_uint(uint32_t to_print){
    char a[13];
    char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    int temp,i=0;
    if(to_print==0)
        kernel_print_string("0",1);

    while (to_print!=0)
    {   
        temp=to_print%10;
        to_print=to_print/10;
        a[i]=digits[temp];
        i++;
    }
    a[i]='\0';
    char c_temp;
    int start=0,end=i-1;
    while (start<end)
    {
        c_temp=a[start];
        a[start]=a[end];
        a[end]=c_temp;
        start++;
        end--;
    }
    kernel_print_string(a,i);
}

// it will print hex of 8 bit no
void kernel_print_char_hex(uint8_t hex){
    char to_hex[16];
    uint8_t temp=hex;
    to_hex[0]='0';
    to_hex[1]='1';
    to_hex[2]='2';
    to_hex[3]='3';
    to_hex[4]='4';
    to_hex[5]='5';
    to_hex[6]='6';
    to_hex[7]='7';
    to_hex[8]='8';
    to_hex[9]='9';
    to_hex[10]='A';
    to_hex[11]='B';
    to_hex[12]='C';
    to_hex[13]='D';
    to_hex[14]='E';
    to_hex[15]='F';
    kernel_terminal_print_char(to_hex[hex>>4]);
    hex=temp;
    kernel_terminal_print_char(to_hex[hex & 15]);
    kernel_terminal_print_char(' ');
    }

void kernel_print_str_hex(void *buffer,uint32_t length){
    uint8_t *temp=(uint8_t *)buffer;
    for(uint32_t i=0;i<length;i++){
        kernel_print_char_hex(temp[i]);
    }
}

void panic(char *ptr){
    kernel_print(ptr);
    while (1)
    {
    }
    
}

void kernel_page()
{
    kernel_registers();
    paging_switch(kernel_chunk->directory_entry);
}

struct tss tss;
struct gdt gdt_real[TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},                // NULL Segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},           // Kernel code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},            // Kernel data segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},              // User code segment
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},             // User data segment
    {.base = (uint32_t)&tss, .limit=sizeof(tss), .type = 0xE9}      //TSS SEGMENT
};

/*
// this is all main function where functions of kernel are added after testing
void kernal_main(){
    terminal_init();
      // physical memory allocate
    // init around 1gb memory and 100mb static to load kernel in it
    // kernel will take more memory if we dynamically allocate
    pmm_init(250000,25000);

}
*/

// this function is only to test kernal flow
void kernel_main(){
    terminal_init();
    memset(gdt_real,0,sizeof(gdt_real));
    idt_init();
    kernel_print("Interrupt Descriptor Table Initialize");
    kernel_print(" ");
   gdt_structured_to_gdt(gdt_real,gdt_structured,TOTAL_GDT_SEGMENTS);
   kernel_print("Global Descriptor Table Initialize");
   kernel_print(" ");
   gdt_load(gdt_real,sizeof(gdt_real));
   
    pmm_init(250000,25000);
    kernel_print("250000 Block of Memory Initialize");
    kernel_print(" ");
    kernel_print("25000 block Reserved for Kernel");
    kernel_print(" ");
    heap_init();
    fs_init();
    disk_search_init();
    kernel_print("Filesystem Detected and Loaded");
    kernel_print(" ");
    //paging
     // Setup paging
    kernel_chunk = paging_new_4gb(PTE_WRITEABLE | PTE_PRESENT | PTE_ACCESS_FROM_ALL);

    // Switch to kernel paging chunk
    paging_switch(paging_4gb_chunk_get_directory(kernel_chunk));

    // Enable paging
    enable_paging();
    kernel_print("Paging Enableded");
    kernel_print(" ");
    // set tss
    memset(&tss,0,sizeof(tss));
    tss.esp0=0x600000;
    tss.ss0=KERNEL_DATA_SELECTOR;
    tss_load(0x28);

    isr80h_register_commands();
    
   // video_memory[0]=make_char('b',15);
   //char* test="hello world";
    //kernel_print_string(test,kernel_str_lenght(test));
    //kernel_print_char_hex(inb(0x01F6));
    
    /*
    memory test done
    char a[5];
    char b[5];
    memset(a,'4',5);
    //memset(b,'4',5);
    memcpy(b,a,5);
    int test=memcmp(a,b,5);
    if(test==0)
    kernel_print_string("workk",5);
    */
   
   //print int
  // kernel_print_uint(123456789);

  // physical memory allocate
    // init around 1gb memory and 100mb static to load kernel in it
    // kernel will take more memory if we dynamically allocate
    
  /*
    uint32_t temp_addr=(uint32_t)pmm_allocate_block();
    kernel_print_uint(temp_addr);
    
   size_t temp;
   kernel_print_uint(sizeof(temp));
   */

  
  /*
    // heap memory test
    void *temp;
   temp=kmallac(12000);
    kernel_print_uint((uint32_t)temp);

   temp=kmallac(65536);
   kernel_print_uint((uint32_t)temp);

   //kfree(temp);

   temp=kmallac(4097);
   kernel_print_uint((uint32_t)temp);

   */
   

  /* disk test

    char buffer[512];
    //kernel_print_char_hex(10);
   // unsigned char temp=inb(0x03F6);
    //temp=10;
    
   // asm("out %eax,%edx");
  // outb(0x01f6,'a');
    kernel_print_char_hex(10);
    
   // kernel_print_char_hex(11);

    disk_search_init();
    disk_read_block(disk_get(0),0,1,buffer);
    kernel_print_str_hex(buffer,512);
  */
 /*
    //path aprser test
    struct path_root* path;
    char *temp="1:/bin/shell.exe";
    path=Pathparser_parse(&temp);
    if(path->first->next->next==0)
        kernel_print_string((char *)path->first->next->part,3);
   // kernel_print_string(path->drive_path);
*/


 /*stremer test
 
    disk_search_init();
    struct disk_stream* streamer=create_disk_streamer(0);
    disk_streamer_seek(streamer,513);
    char buff[512];
    kernel_print_char_hex(10);
    disk_streamer_read(streamer,buff,2);
    kernel_print_str_hex(buff,2);
    */

   /* FAT 16 test
    VFS layer fopen
    fat16_resolve(disk_get(0));
    char *name="0:/file.txt";
    //char out_temp[40];
    //char *out=out_temp;
    //fat16_to_proper_string(&out,name);
   // kernel_print_uint( istrncmp("hell","HELL",4));
   int fd=fopen(name,"r");
   fd=fopen("0:/hello.txt","r");
    kernel_print_uint(fd);
    */
   
   // VFS test

   struct process* p=0;
    process_load("0:/blank.bin",&p);
    struct process* p1=0;
    process_load("0:/blank1.bin",&p1);
    struct process* p2=0;
    process_load("0:/blank2.bin",&p2);
   task_run_first_ever_task();
   kernel_print("here");
}
