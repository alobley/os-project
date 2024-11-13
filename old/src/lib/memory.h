#ifndef MEMORY_H
#define MEMORY_H

#define IDT_BASE 0x00001000

#include "types.h"

// Emergencies only - clear the kernel's heap entirely
void PANIC_FREE_HEAP();

// Initialize the kernel's memory heap
void InitializeMemory();

// Kernel malloc
void* kmalloc(size_t size);

// Kernel free
void kfree(void *ptr);

uint32 GetMemSize();

#endif