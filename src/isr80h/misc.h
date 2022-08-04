#ifndef ISR80H_MISC_H
#define ISR80H_MISC_H
#include "../idt/idt.h"

struct interrupt_frame;
void* isr80h_command0_sum(struct interrupt_frame* frame);
void* isr80h_command4_sys_write(struct interrupt_frame* frame);
#endif