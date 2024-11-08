#ifndef ASM_H
#define ASM_H

#define hlt() asm ("hlt")
#define cli() asm volatile("cli")
#define sti() asm volatile("sti" ::: "memory")
#define nop() asm volatile("nop")

#endif