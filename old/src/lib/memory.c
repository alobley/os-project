#include "memory.h"
#include "drivers/console.h"
#include "asm.h"
#include "io.h"

extern uint32 __kernel_end;

uint32 KERNEL_FREE_HEAP_BEGIN;
uint32 KERNEL_FREE_HEAP_END;


typedef struct memory_block {
    size_t size;                 // Size of the memory block
    struct memory_block* next;   // Pointer to the next free block of memory
    bool free;                   // Block free or bot
} memory_block_t;

#define MEMORY_BLOCK_SIZE (sizeof(memory_block_t))

memory_block_t* kernel_heap;

uint32 kernel_heap_end;

uint32 memsize;

uint32 GetMemSize(){
    return memsize;
}

uint32 MemDetect(){
    uint32 total;
    uint8 lowmem, midlow, midhigh, highmem;

    outb(0x70, 0x30);
    lowmem = inb(0x71);
    midlow = inb(0x71);
    midhigh = inb(0x71);
    highmem = inb(0x71);

    total = lowmem | (midlow << 8) | (midhigh << 16) | (highmem << 24);

    kprintf("%llu\n", total);

    return total;
}

void InitializeMemory(){
    // Set to 1 billion bytes for now (GRUB is being dumb and stupid)
    kernel_heap_end = 1000000000;

    KERNEL_FREE_HEAP_BEGIN = __kernel_end;
    kernel_heap = (memory_block_t *)KERNEL_FREE_HEAP_BEGIN;

    kernel_heap->size = KERNEL_FREE_HEAP_END - KERNEL_FREE_HEAP_BEGIN - MEMORY_BLOCK_SIZE;
    kernel_heap->next = NULL;
    kernel_heap->free = true;

    memsize = kernel_heap_end;
}

void* kmalloc(size_t size){
    memory_block_t* current = kernel_heap;
    memory_block_t* prev = NULL;

    while(current != NULL){
        if(current->free && current->size >= size){
            // Suitable free block found
            if(current->size >= size + MEMORY_BLOCK_SIZE + 1){
                // Split the block if it's big enough
                memory_block_t* new_block = (memory_block_t* )((uint8* )current + MEMORY_BLOCK_SIZE + size);
                new_block->size = current->size - size - MEMORY_BLOCK_SIZE;
                new_block->next = current->next;
                new_block->free = true;

                current->size = size;
                current->next = new_block;
            }
            current->free = false;
            return (void *)((uint8 *)current + MEMORY_BLOCK_SIZE);
        }
        prev = current;
        current = current->next;
    }

    // No suitable block found, expand heap
    if(kernel_heap_end + size + MEMORY_BLOCK_SIZE < KERNEL_FREE_HEAP_END){
        memory_block_t* new_block = (memory_block_t* )kernel_heap_end;
        new_block->size = size;
        new_block->next = NULL;
        new_block->free = false;

        if(prev != NULL){
            prev->next = new_block;
        }else{
            kernel_heap = new_block;
        }

        kernel_heap_end += size + MEMORY_BLOCK_SIZE;
        return (void* )((uint8* )new_block + MEMORY_BLOCK_SIZE);
    }

    // Out of memory
    return NULL;
}

void kfree(void* ptr){
    if(ptr == NULL){
        // Data does not exist
        return;
    }

    memory_block_t* block = (memory_block_t* )((uint8* )ptr - MEMORY_BLOCK_SIZE);
    block->free = true;

    // Coalesce adjacent free blocks to prevent memory fragmentation
    memory_block_t* current = kernel_heap;
    while(current != NULL){
        if(current->free && current->next != NULL && current->next->free){
            current->size += current->next->size + MEMORY_BLOCK_SIZE;
            current->next = current->next->next;
        }
        current = current->next;
    }
}