.section .text.prologue
.global _start
.type _start, @function

_start:
    // Start the kernel
    call kernel_main

    // If we ever return (which we shouldn't), stop the processor.
    cli
    jmp hang
hang:
    hlt
    jmp hang

.section .text
.align 8

.section .data
.align 32
// Doesn't work without this. Idk why.
empty:
    .fill 0x4000
endempty:
