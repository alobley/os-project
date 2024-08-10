OUTPUT_FORMAT("binary")
SECTIONS
{
    localApicAddr = 0x00006000;
    
    .text 0x0000000000007E00:
    {
        *(.text.prologue)

        *(.text)

        *(.text.interrupts)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
        __stack_start = .;
    }
}