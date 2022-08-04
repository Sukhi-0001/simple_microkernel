#include "scheduler.h"
#include "../kernel.h"
#include "../idt/idt.h"
#include "../task/task.h"

void schedule_next_task(struct interrupt_frame* frame)
{   
    struct task* new_task=task_get_next();
    task_switch(new_task);
    task_return(&new_task->registers);
}