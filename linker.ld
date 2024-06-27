OUTPUT_FORMAT("binary)
ENTRY(kernel_main)
SECTIONS
{
    localApicAddr = 0x00006000;
    
    .text :
    {
        *(.text)
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