#ifndef IDT_H
#define IDT_H
#define TOTAL_INTERRUPTS 512
#include <stdint.h>
#define MAX_ISR80H_COMMANDS 1024
struct interrupt_frame;

typedef void*(*ISR80H_COMMAND)(struct interrupt_frame* frame);
void isr80h_register_command(int command_id, ISR80H_COMMAND command);
struct idt_desc
{
    uint16_t offset_1; // Offset bits 0 - 15
    uint16_t selector; // Selector thats in our GDT
    uint8_t zero; // Does nothing, unused set to zero
    uint8_t type_attr; // Descriptor type and attributes
    uint16_t offset_2; // Offset bits 16-31
} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // Size of descriptor table -1
    uint32_t base; // Base address of the start of the interrupt descriptor table
} __attribute__((packed));

struct interrupt_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    //this are pushed by cpu
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
} __attribute__((packed));

void idt_init();
void idt_load(struct idtr_desc* ptr);

#endif