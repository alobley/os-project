#ifndef IO_H
#define IO_H

#include "types.h"

uint8 inb(uint16 port);
void outb(uint16 port, uint8 data);
uint16 inw(uint16 port);
void outw(uint16 port, uint16 data);
uint32 inl(uint16 port);
void outl(uint16 port, uint32 data);

#endif  // IO_H