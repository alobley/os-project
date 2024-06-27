#include "idt.h"
#include "../memory.h"

typedef struct IdtDesc{
    uint16 limit;
    uint64 base;
} __attribute__((packed)) IdtDesc;


typedef struct IdtEntry {
    uint16 offset0;
    uint16 selector;
    uint16 type;
    uint16 offset1;
    uint32 offset2;
    uint32 reserved;
} __attribute__((packed)) IdtEntry;

typedef void (*Handler)();

extern void default_exception_handler();
extern void default_interrupt_handler();
extern void (*exception_handlers[20])();

void IdtInit(){
    for(uint32 i = 0; i < 20; ++i){
        IdtSetHandler(i, INTERRUPT_GATE, exception_handlers[i]);
    }

    for(uint32 i = 20; i < 32; ++i){
        IdtSetHandler(i, INTERRUPT_GATE, default_exception_handler);
    }

    for(uint32 i = 32; i < 256; ++i){
        IdtSetHandler(i, TRAP_GATE, default_interrupt_handler);
    }

    IdtDesc idtDesc = {
        .limit = 256 * sizeof(IdtEntry) - 1,
        .base = IDT_BASE
    };

    asm volatile("lidt %0" : : "m" (idtDesc) : "memory");
}

static void SetIdtEntry(uint8 index, uint64 base, uint16 selector, uint16 type){
    IdtEntry *entry = (IdtEntry *)IDT_BASE + index;

    entry->offset0 = (uint16)base;
    entry->selector = selector;
    entry->type = type;
    entry->offset1 = (uint16)(base >> 16);
    entry->offset2 = (uint32)(base >> 32);
    entry->reserved = 0;
}

void IdtSetHandler(uint8 index, uint16 type, void (*handler)()){
    if(handler){
        uint16 selector = 0x8;
        SetIdtEntry(index, (uint64)handler, selector, type);
    }else{
        SetIdtEntry(index, 0, 0, 0);
    }
}