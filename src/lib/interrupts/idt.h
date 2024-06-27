#ifndef IDT_H
#define IDT_H

#include "../types.h"
#define INTERRUPT_GATE 0x8E00
#define TRAP_GATE 0x8F00

void IdtInit();
void IdtSetHandler(uint8 index, uint16 type, void (*handler)());

#endif