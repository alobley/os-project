#ifndef IDT_H
#define IDT_H

#include "../types.h"
#define INTERRUPT_GATE 0x8E00
#define TRAP_GATE 0x8F00

typedef struct IdtDesc{
    uint16 limit;
    uint64 base;
} __attribute__((packed)) IdtDesc;


typedef struct IdtEntry {
    uint16 offset0;
    uint16 selector;
    uint16 type;
    uint16 offset1;
    uint32 offset2;
    uint32 reserved;
} __attribute__((packed)) IdtEntry;

void IdtInit();
void IdtSetHandler(uint8 index, uint16 type, void (*handler)());

#endif