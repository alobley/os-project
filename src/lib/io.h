#ifndef IO_H
#define IO_H

// Get a byte from a specified port
unsigned char inb(unsigned short port);

// Send a byte to a specified port
void outb(unsigned short port, unsigned char data);

// Get a dword from a specified port
unsigned int inl(unsigned short port);

// Send a dword to a specified port
void outl(unsigned short port, unsigned int data);


#endif