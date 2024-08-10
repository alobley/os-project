BITS 32
CPU x64

section .text.prologue
extern kernel_main
extern __stack_start

; Looks like we get 2 GiB out of this. It should be fine for a while. I hope.
enable_paging:
    mov eax, p3_table
    or eax, 0b11
    mov dword [p4_table + 0], eax

    mov eax, p2_table
    or eax, 0b11
    mov dword [p3_table + 0], eax

    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jne .map_p2_table
    jmp finish


finish:
    mov eax, p4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    or eax, 1 << 16
    mov cr0, eax

    lgdt [gdt64.pointer]

    ; Jump to the kernel
    jmp gdt64.code:start

BITS 64

start:
    cli
    mov ax, gdt64.data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rbp, __stack_start
    lea rsp, [__stack_start + 4096]

    jmp kernel_main

section .rodata
gdt64:
.null: equ $ - gdt64
    dq 0
.code: equ $ - gdt64
    dd 0xFFFF
    db 0
    db (1 << 7) | (1 << 4) | (1 << 3) | (1 << 1)
    db (1 << 7) | (1<<5) | 0xF
    db 0
.data: equ $ - gdt64
    dd 0xFFFF
    db 0
    db (1 << 7) | (1 << 4) | (1 << 1)
    db (1 << 7) | (1 << 6) | 0xF
    db 0
.tss: equ $ - gdt64
    dd 0x00000068
    dd 0x00CF8900
.pointer:
    dw .pointer - gdt64 - 1
    dq gdt64

section .bss
ALIGN 4096

p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096

ALIGN 16
stack_bottom:
    resb 4096
stack_top: