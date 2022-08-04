#include "misc.h"
#include "../idt/idt.h"
#include "../task/task.h"
#include "../kernel.h"


void* isr80h_command0_sum(struct interrupt_frame* frame){
    int res=(int)task_get_stack_item(task_current(),0);
    kernel_print_uint(res);
    panic("in 0 command");
return 0;
}

void* isr80h_command4_sys_write(struct interrupt_frame* frame){
    struct task* currt_task=task_current();
    char buffer[512];
    copy_string_from_task(currt_task,(uint32_t*)currt_task->registers.ebx,buffer,512);
    kernel_print(buffer);
    //kernel_print("in kernel 80h sys write");
return 0;
}

