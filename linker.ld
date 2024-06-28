OUTPUT_FORMAT("binary")
ENTRY(kernel_main)
SECTIONS
{
    localApicAddr = 0x00006000;
    
    .text 0x0000000000007E00:
    {
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
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    }
}