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

static inline void mmiowriteb(void *p, unsigned char data){
    *(volatile unsigned char *)(p) = data;
}

static inline unsigned char mmioreadb(void *p){
    return *(volatile unsigned char *)(p);
}

static inline void mmiowritel(void *p, unsigned int data){
    *(volatile unsigned int *)(p) = data;
}

static inline unsigned int mmioreadl(void *p){
    return *(volatile unsigned int *)(p);
}

#endif