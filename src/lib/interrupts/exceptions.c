#include "exceptions.h"
#include "../vga.h"

static const char exceptionDescs[20][28] = {
    [0] = "Divide By 0",
    [1] = "Debug",
    [2] = "Nonmaskable Interrupt",
    [3] = "Breakpoint",
    [4] = "Overflow",
    [5] = "Bound Range Exception",
    [6] = "Invalid Opcode",
    [7] = "Device Not Available",
    [8] = "Double Fault",
    [9] = "Coprocessor Segment Overrun",
    [10] = "Invalid TSS",
    [11] = "Segment Not Present",
    [12] = "Stack Segment Fault",
    [13] = "General Protection Fault",
    [14] = "Page Fault",
    [16] = "Floating Point Error",
    [17] = "Alignment Check",
    [18] = "Machine Check",
    [19] = "SIMD Exception"
};

uint32 strlen(const char* string){
    uint32 len = 0;
    while(*(string + len) != '\0'){
        len++;
    }
    return len;
}

void ExceptionDump(Registers regs){
    ClearVGAMem();

    const char *desc = "Unknown";
    if(regs.intNum < 20){
        desc = exceptionDescs[regs.intNum];
    }

    WriteStr(desc, 0, 0);
    uint32 nextCharPos = strlen(desc);

    WriteStr(" Interrupt", nextCharPos, 0);

    for(;;);
}