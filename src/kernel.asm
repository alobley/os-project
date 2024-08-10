BITS 32
org 0x7e00

section .text
    global _bootstrap

; Prepare the stack and call the kernel_main function
_bootstrap:
    mov esp, kernel_stack_top
    and esp, -16
    mov ebp, esp

    mov eax, 0xDEADBEEF
    push eax
    sub ebp, 4

    cli
    call kernel_main

%define VGA_TEXT_MODE_START 0xB8000
%define VGA_TEXT_MODE_SCREEN_SIZE 4000

%define VGA_PIXEL_MODE_START 0xA0000
%define VGA_PIXEL_MODE_SCREEN_SIZE 64000

; The kernel entry point
kernel_main:
    call screen_init
    
    mov al, 0xFF
    call fill_screen
end:
    hlt
    jmp end


fill_screen:
    mov esi, VGA_PIXEL_MODE_START
    mov ecx, VGA_PIXEL_MODE_SCREEN_SIZE
.loop:
    mov byte [esi], al
    inc esi
    dec ecx
    cmp ecx, 0
    jne .loop
    ret

%define PALATTE_MASK 0x3C6
%define PALATTE_READ 0x3C7
%define PALATTE_WRITE 0x3C8
%define PALATTE_DATA 0x3C9


; Enable VGA pixel mode

screen_init:
    ; configure palette with 8-bit RRRGGGBB color
    mov dx, PALATTE_MASK
    mov al, 0xFF
    out dx, al

    mov dx, PALATTE_WRITE
    mov al, 0
    out dx, al

    mov ecx, 0         ; Initialize loop counter

palette_loop:
    movzx eax, cl      ; Move loop counter to EAX register zero-extending it to 32 bits
    shr eax, 5         ; Shift right by 5 bits to get R component
    and eax, 0x7       ; Mask with 0x7 to get R component in range [0, 7]
    imul eax, eax, 32  ; Multiply R component by (256/8)
    shr eax, 2         ; Divide by 4
    mov dx, PALATTE_DATA
    out dx, al         ; Write R component

    movzx eax, cl      ; Move loop counter to EAX register zero-extending it to 32 bits
    shr eax, 2         ; Shift right by 2 bits to get G component
    and eax, 0x7       ; Mask with 0x7 to get G component in range [0, 7]
    imul eax, eax, 32  ; Multiply G component by (256/8)
    shr eax, 2         ; Divide by 4
    mov dx, PALATTE_DATA
    out dx, al         ; Write G component

    movzx eax, cl      ; Move loop counter to EAX register zero-extending it to 32 bits
    and eax, 0x3       ; Mask with 0x3 to get B component
    imul eax, eax, 64  ; Multiply B component by (256/4)
    shr eax, 2         ; Divide by 4
    mov dx, PALATTE_DATA
    out dx, al         ; Write B component

    inc cl             ; Increment loop counter
    cmp cl, 255        ; Compare loop counter with 255
    jne palette_loop   ; Jump back to palette_loop if not equal

    ; set color 255 = white
    mov dx, PALATTE_DATA
    mov al, 0x3F
    out dx, al         ; Write R component

    mov dx, PALATTE_DATA
    mov al, 0x3F
    out dx, al         ; Write G component

    mov dx, PALATTE_DATA
    mov al, 0x3F
    out dx, al         ; Write B component

    ret



section .data
msg: db "Hello, Kernel!", 0

section .bss
kernel_stack_bottom:
    resb 0x4000
kernel_stack_top: