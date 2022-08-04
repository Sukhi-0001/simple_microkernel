section .asm
extern no_interrupt_handler
global no_interrupt

global isr_0x20_wrapper
extern isr_0x20_handler

global isr80h_wrapper
extern isr80h_handler

global idt_load
idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

no_interrupt:
    cli
    pushad
    call no_interrupt_handler
    popad
    sti
    iret

isr80h_wrapper:
    ; INTERRUPT FRAME START
    ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
    ; uint32_t ip
    ; uint32_t cs;
    ; uint32_t flags
    ; uint32_t sp;
    ; uint32_t ss;
    ; Pushes the general purpose registers to the stack
    pushad
    ; INTERRUPT FRAME END

    ; Push the stack pointer so that we are pointing to the interrupt frame
    push esp

    ; EAX holds our command lets push it to the stack for isr80h_handler
    push eax
    call isr80h_handler
    mov dword[tmp_res], eax
    add esp, 8

    ; Restore general purpose registers for user land
    popad
    mov eax, [tmp_res]
    iret

isr_0x20_wrapper:
    ; INTERRUPT FRAME START
    ; ALREADY PUSHED TO US BY THE PROCESSOR UPON ENTRY TO THIS INTERRUPT
    ; uint32_t ip
    ; uint32_t cs;
    ; uint32_t flags
    ; uint32_t sp;
    ; uint32_t ss;
    ; Pushes the general purpose registers to the stack
    pushad
    ; INTERRUPT FRAME END

    ; Push the stack pointer so that we are pointing to the interrupt frame
    push esp
    
    call isr_0x20_handler
    add esp, 4

    ; Restore general purpose registers for user land
    popad
    mov eax,[temp_eax]
    mov al,0x20
    out 0x20,al
    mov [temp_eax],eax
    iret

section .data
; Inside here is stored the return result from isr80h_handler
tmp_res: dd 0
temp_eax: dd 0
