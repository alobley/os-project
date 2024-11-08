#ifndef IDT_H
#define IDT_H

#include "../types.h"
#include "isr.h"

void SetIDT(uint8 index, void (*base)(struct Registers*), uint16 selector, uint8 flags);

void InitIDT();

#endif