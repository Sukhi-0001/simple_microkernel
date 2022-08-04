#ifndef PROCESS_H
#define PROCESS_H
#include <stdint.h>
#include "task.h"
#include "../fs/file.h"
#include "task.h"

#define MAX_PROGRAM_ALLOCATIONS 1024
#define MAX_PROCESSES 12

struct process
{
    // The process id
    uint16_t id;

    char filename[MAX_PATH];

    // The main process task
    struct task* task;

    // The memory (malloc) allocations of the process
    void* allocations[MAX_PROGRAM_ALLOCATIONS];

    // The physical pointer to the process memory.
    void* ptr;

    // The physical pointer to the stack memory
    void* stack;

    // The size of the data pointed to by "ptr"
    uint32_t size;

};

int process_load_for_slot(char* filename, struct process** process, int process_slot);
int process_load(char* filename,struct process**process);

#endif