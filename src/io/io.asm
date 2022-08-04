BITS 32
section .asm
global inb
global inw
global outb
global outw

inb:
    push ebp
    mov ebp,esp

    xor eax,eax
    mov edx,[ebp+8] ; 1st argument pass by function 
    in al,dx

    pop ebp
    ret

inw:
    push ebp
    mov ebp,esp
    xor eax,eax
    mov edx,[ebp+8] ;1st argument io des address
    in ax,dx        ;in dx address
    pop ebp
    ret

outb:
    push ebp
    mov ebp, esp

    mov eax, [ebp+12]
    mov edx, [ebp+8]
    out dx, al

    pop ebp
    ret

outw:
    push ebp
    mov ebp,esp
    xor eax,eax

    mov edx,[ebp+8] ;1st argument io des add
    mov eax,[ebp+12] ;2nt argument data to send io device
    out dx,ax
    pop ebp
    ret
