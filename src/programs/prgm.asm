section .text
    global _start

_start:
    ; Perform a system call and return. That's all this needs to do.
    mov eax, 1
    int 0x30
    ret