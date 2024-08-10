#include "exceptions.h"
#include "../drivers/vga.h"
#include "../asm.h"

static const char exceptionDescs[20][28] = {
    [0] = "Divide By 0\n",
    [1] = "Debug\n",
    [2] = "Nonmaskable Interrupt\n",
    [3] = "Breakpoint\n",
    [4] = "Overflow\n",
    [5] = "Bound Range Exception\n",
    [6] = "Invalid Opcode\n",
    [7] = "Device Not Available\n",
    [8] = "Double Fault\n",
    [9] = "Coprocessor Segment Overrun\n",
    [10] = "Invalid TSS\n",
    [11] = "Segment Not Present\n",
    [12] = "Stack Segment Fault\n",
    [13] = "General Protection Fault\n",
    [14] = "Page Fault\n",
    [16] = "Floating Point Error\n",
    [17] = "Alignment Check\n",
    [18] = "Machine Check\n",
    [19] = "SIMD Exception\n"
};

void ExceptionDump(Registers regs){
    ClearTerminal();

    const char *desc = "Unknown";
    if(regs.intNum < 20){
        desc = exceptionDescs[regs.intNum];
    }

    kprintf(desc);
    uint32 nextCharPos = strlen(desc);

    for(;;);
}