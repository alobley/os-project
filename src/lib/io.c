#include "io.h"

// Get a byte of data from a port
uint8 inb(uint16 port){
    uint8 data;
    asm volatile("in %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// Send a byte of data to a port
void outb(uint16 port, uint8 data){
    asm volatile("out %0, %1" : : "a" (data), "Nd" (port));
}

// Get 16 bits of data from a port
uint16 inw(uint16 port){
    uint16 data;
    asm volatile("in %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// Send 16 bits of data out a port
void outw(uint16 port, uint16 data){
    asm volatile("out %0, %1" : : "a" (data), "Nd" (port));
}

// Get a 32-bit piece of data from a port
uint32 inl(uint16 port){
    uint32 data;
    asm volatile("in %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// Send 32 bits of data out to a port
void outl(uint16 port, uint32 data){
    asm volatile("out %0, %1" : : "a" (data), "Nd" (port));
}