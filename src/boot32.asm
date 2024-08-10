BITS 16
org 0x7c00

LOAD_ADDRESS: equ 0x7e00
TOTAL_SECTORS: equ 10000
STACK_TOP: equ 0x900000

start: jmp boot

gdt_start:
gdt_null:
    dq 0x0
gdt_kernel_code:
    dw 0xFFFF               ; Limit (the maximum addressable segment of this region, set to 0xFFFFF for all 4 gigs)
    dw 0x0000               ; Base (where this starts in physical memory) 
    db 0x00                 ; Padding?
    db 0b10011011           ; Access Byte (contains flags on what defines this segment, details at https://wiki.osdev.org/Global_Descriptor_Table)
    db 0b11001111           ; Flags (defines certain aspects of the segment)
    db 0x00                 ; Padding?
gdt_kernel_data:
    dw 0xFFFF               ; Limit (OSDev wiki LIED)
    dw 0x0000               ; Base
    db 0x00                 ; Padding?
    db 0b10010011           ; Access Byte
    db 0b11001111           ; Flags
    db 0x00                 ; Padding?

gdt_end:

gdtr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG: equ gdt_kernel_code - gdt_start
DATA_SEG: equ gdt_kernel_data - gdt_start

idt:
    dw 0x0
    dq 0x0

drive_num: db 0

boot:
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    cld

    mov byte [drive_num], dl

    mov si, boot_msg
    call print_string

    call load_disk

    jmp enable_pm


; Reads the kernel (aka the entire disk) into memory
load_disk:
    mov ah, 0x02
    mov al, 0x13
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [drive_num]
    mov bx, LOAD_ADDRESS
    int 0x13
    mov ah, 0x00

    mov cx, TOTAL_SECTORS
.readloop:
    int 0x13
    
    add bx, 512
    inc dl
    dec cx
    jnz .readloop
    ret

print_string:
    mov ah, 0x0e
.loop:
    mov al, [si]
    int 0x10
    inc si
    cmp byte [si], 0
    je .done
    jmp .loop
.done:
    ret

err_str: db "Error loading kernel", 0
boot_msg: db "Booting...", 0

enable_pm:
    cli

    push ds
    push es

    in al, 0x92
    or al, 0x02
    out 0x92, al

    lgdt [gdtr]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    pop es
    pop ds

    sti

    jmp CODE_SEG:protected_mode

BITS 32
protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Move the stack to an empty memory region
    mov ebp, STACK_TOP
    mov esp, ebp

    lidt[idt]

    mov eax, 0xFFFFFFFF

    jmp LOAD_ADDRESS



times 510 - ($-$$) db 0
dw 0xAA55