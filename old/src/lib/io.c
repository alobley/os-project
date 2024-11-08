#include "io.h"

// Get a byte in from a port
unsigned char inb(unsigned short port){
    unsigned char data;
    asm volatile("in %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// Send a byte out to a port
void outb(unsigned short port, unsigned char data){
    asm volatile("out %0, %1" : : "a" (data), "Nd" (port));
}

unsigned short inw(unsigned short port){
    unsigned short data;
    asm volatile("in %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

void outw(unsigned short port, unsigned short data){
    asm volatile("out %0, %1" : : "a" (data), "Nd" (port));
}

// Get a 32-bit piece of data from a port
unsigned int inl(unsigned short port){
    unsigned int data;
    asm volatile("in %1, %0" : "=a" (data) : "Nd" (port));
    return data;
}

// Send 32 bits of data out to a port
void outl(unsigned short port, unsigned int data){
    asm volatile("out %0, %1" : : "a" (data), "Nd" (port));
}

void mmiowriteb(void *p, unsigned char data){
    *(volatile unsigned char *)(p) = data;
}

unsigned char mmioreadb(void *p){
    return *(volatile unsigned char *)(p);
}

void mmiowritel(void *p, unsigned int data){
    *(volatile unsigned int *)(p) = data;
}

unsigned int mmioreadl(void *p){
    return *(volatile unsigned int *)(p);
}