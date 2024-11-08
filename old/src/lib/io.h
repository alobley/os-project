#ifndef IO_H
#define IO_H

#include "types.h"

// Get a byte from a specified port
unsigned char inb(unsigned short port);

// Send a byte to a specified port
void outb(unsigned short port, unsigned char data);

unsigned short inw(unsigned short port);

void outw(unsigned short port, unsigned short data);

// Get a dword from a specified port
unsigned int inl(unsigned short port);

// Send a dword to a specified port
void outl(unsigned short port, unsigned int data);

void mmiowriteb(void *p, unsigned char data);

unsigned char mmioreadb(void *p);

void mmiowritel(void *p, unsigned int data);

unsigned int mmioreadl(void *p);

#endif