#include "memory.h"
#include "types.h"

// Define the kernel memory heap. It's the last gigabyte of RAM in a 4GB system. At some point, I will probably implement dynamic allocation.
#define KERNEL_FREE_HEAP_BEGIN 3000000000ULL
#define KERNEL_FREE_HEAP_END 4000000000ULL

// Defines a block of memory that the kernel has either allocated or can use
typedef struct mem_block{
    size_t size;
    struct mem_block* next;
} mem_block_t;

// Set the free region of memory for allocating
static mem_block_t* free_list = (mem_block_t* )KERNEL_FREE_HEAP_BEGIN;

// Initialize the kernel's memory heap
void init_memory(){
    free_list->size = KERNEL_FREE_HEAP_END - KERNEL_FREE_HEAP_BEGIN;
    free_list->next = NULL;
}

// Emergency function that clears the kernel's memory heap
void PANIC_FREE_HEAP(){
    free_list = (mem_block_t* )KERNEL_FREE_HEAP_BEGIN;
    free_list->size = KERNEL_FREE_HEAP_END - KERNEL_FREE_HEAP_BEGIN;
    free_list->next = NULL;
}

// Allows the kernel to allocate a specific amount of memory in its 1G heap
void* kmalloc(size_t size){
    mem_block_t* current = free_list;
    mem_block_t* previous = NULL;

    // Align to the nearest multiple of 8
    size = (size + 7) & ~7;

    size_t totalSize = size + sizeof(mem_block_t);
    while(current){
        // Check if we've reached the end of free space. If so, return null.
        if((uint64)current + current->size >= KERNEL_FREE_HEAP_END){
            return NULL;
        }

        // If we have not run out of space, allocate an area of the kernel's memory heap, to as much as is needed.
        if(current->size >= totalSize){
            if(current->size > totalSize + sizeof(mem_block_t)){
                // Split the block
                mem_block_t* new_block = (mem_block_t*)((uint64)current + totalSize);
                new_block->size = current->size - totalSize;
                new_block->next = current->next;
                current->size = size;
                current->next = NULL;
                
                if(previous){
                    // If we created a new memory block
                    previous->next = new_block;
                }else{
                    free_list = new_block;
                }
            }else{
                if(previous){
                    // If we created a new memory block
                    previous->next = current->next;
                }else{
                    free_list = current->next;
                }
            }
            return (void*)((uint64)current + sizeof(mem_block_t));
        }

        // The current block of memory was insufficient, allocate a new block
        previous = current;
        current = current->next;
    }

    // No suitable block found
    return NULL;
}


// Allows the kernel to free heap space
void kfree(void* ptr){
    // If the pointer is null, nothing needs to be freed.
    if(!ptr) return;

    // Get the block of memory the allocated memory is located in
    mem_block_t* block = (mem_block_t*)((uint64)ptr - sizeof(mem_block_t));

    // Prepare the free memory for expansion
    mem_block_t* current = free_list;
    mem_block_t* previous = NULL;

    // Coalesce free blocks of memory to prevent fragmentation and clear them
    while(current && (uint64)current < (uint64)block){
        previous = current;
        current = current->next;
    }

    if(previous){
        if((uint64)previous + previous->size == (uint64)block){
            previous->size += block->size;
            block = previous;
        }else{
            previous->next = block;
        }
    }else{
        free_list = block;
    }

    if(current && (uint64)block + block->size == (uint64)current){
        block->size += current->size;
        block->next = current->next;
    }else{
        block->next = current;
    }
}

