#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include "../types.h"
typedef struct Registers {
    uint64 rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64 intNum, errCode;
    uint64 rip, cs, eflags, rsp, ss;
} Registers;

void ExceptionDump(Registers regs);

#endif