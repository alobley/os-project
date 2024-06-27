#ifndef MEMORY_H
#define MEMORY_H

#define IDT_BASE 0x00001000

// Emergencies only - clear the kernel's heap entirely
void PANIC_FREE_HEAP();

// Initialize the kernel's memory heap
void init_memory();

// Kernel malloc
void* kmalloc(unsigned long long size);

// Kernel free
void kfree(void *ptr);

#endif