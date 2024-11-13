#ifndef ISR_H
#define ISR_H

#include <types.h>

struct Registers{
    uint32 __ignored, fs, es, ds;
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32 intNum, errNum;
    uint32 eip, cs, efl, useresp, ss;
};

void InstallISR(size_t i, void(*handler)(struct Registers*));
void InitISR();

#endif