#include "process.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include "../memory/heap/kheap.h"
#include "../memory/paging/paging.h"
#include "../fs/file.h"
#include "../string/string.h"

struct process* current_process=0;

struct process* processes[MAX_PROCESSES];

void process_init(struct process* process){
    memset(process,0,sizeof(struct process));
}

struct process* process_get(int process_id)
{
    if (process_id < 0 || process_id >= MAX_PROCESSES)
    {
        return 0;
    }

    return processes[process_id];
}

int process_get_free_slot(){
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        if(processes[i]==0)
            return i;
    }
    return -1;
}

int process_load(char* filename,struct process**process){
    int slot=process_get_free_slot();
    if(slot<0)
        return -1;
    if(process_load_for_slot(filename,process,slot)==1)
        return slot;
    return -1;
}

int process_load_binary(char* filename,struct process* process){
    int fd=fopen(filename,"r");
    if(fd<=0)
        return -1;

    struct file_stat f_stat;
    int res=fstat(fd,&f_stat);
    
    // file is opening till here
    if(res<=0)
        return -1;
    
    
    void *program_data_pointer=kzalloc(f_stat.filesize);
    if(!program_data_pointer){
        return -1;
      
    }

    //problem here
    if(fread(program_data_pointer,f_stat.filesize,1,fd)<=0){
        return -1;
    }
    
    process->ptr=program_data_pointer;
    process->size=f_stat.filesize;
    //fclose(fd);
    return 1;
}

int process_load_data(char * filename,struct process* process){
    int res=0;
    res=process_load_binary(filename,process);
    return res;
}

int process_map_binary(struct process* process){
    int res=0;
    paging_map_to(process->task->page_directory->directory_entry, (void*) PROGRAM_VIRTUAL_ADDRESS, process->ptr, paging_align_address(process->ptr + process->size), PTE_PRESENT | PTE_ACCESS_FROM_ALL | PTE_WRITEABLE);
    if (res < 0)
    {
        return -1;
    }

    paging_map_to(process->task->page_directory->directory_entry, (void*)PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack+USER_PROGRAM_STACK_SIZE), PTE_PRESENT | PTE_ACCESS_FROM_ALL | PTE_WRITEABLE);
    return res;
}

int process_map_memory(struct process* process){
    int res=0;
    res=process_map_binary(process);
    // map stack
    paging_map_to(process->task->page_directory->directory_entry,(void*)PROGRAM_VIRTUAL_STACK_ADDRESS_END, process->stack, paging_align_address(process->stack+USER_PROGRAM_STACK_SIZE), PTE_PRESENT | PTE_ACCESS_FROM_ALL | PTE_WRITEABLE);
    return res;
}

int process_load_for_slot(char* filename, struct process** process, int process_slot){
    
    struct task* task = 0;
    struct process* _process;
    void* program_stack_ptr = 0;

    if (process_get(process_slot) != 0)
    {
        return -1;
        
    }

    
    _process = kzalloc(sizeof(struct process));
    if (!_process)
    {
        return -1;
    }

    // set structure to zero
    process_init(_process);

    int res = process_load_data(filename, _process);
    if (res < 0)
    {  
        return -1;
    }

    // stack grow is to it
    program_stack_ptr=kzalloc(USER_PROGRAM_STACK_SIZE);
    if(!program_stack_ptr)
        return -1;
    
    strncpy(_process->filename,filename,sizeof(_process->filename));
    _process->stack=program_stack_ptr;
    _process->id=process_slot;

    //create task
    task=task_new(_process);
   // kernel_print("we are here till now");
    if(task==0)
        return -1;
    _process->task=task;

    
    res=process_map_memory(_process);
    if(res<0)
        return -1;

    *process=_process;

    // add process to array
    processes[process_slot]=_process;
    return 1;
    //free process data

}