[BITS 32]
section .asm

global _start

_start:

label:
    mov eax,0x2
    mov ebx,0x4
    add eax,ebx
    mov eax,4
    mov ebx,message
    int 80h
    mov eax,0
    jmp label

    section .data
message: db 'I am in Process 0', 0