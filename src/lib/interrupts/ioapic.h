#ifndef IOAPIC_H
#define IOAPIC_H

#include "../types.h"

extern uint8 *localApicAddr;

void IoApicInit();
void IoApicSetEntry(uint8 *base, uint8 index, uint64 data);

#endif