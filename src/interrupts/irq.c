#include "irq.h"
#include "idt.h"
#include "isr.h"
#include <io.h>
#include <util.h>

#define PIC_WAIT() do {         \
        asm volatile ("jmp 1f\n\t"       \
                "1:\n\t"        \
                "    jmp 2f\n\t"\
                "2:");          \
    } while (0)

static void (*handlers[32])(struct Registers *regs) = {0};

static void stub(struct Registers *regs){
    if(regs->intNum <= 47 && regs->intNum >= 32){
        if(handlers[regs->intNum - 32]){
            handlers[regs->intNum - 32](regs);
        }
    }

    if(regs->intNum >= 0x40){
        outb(PIC2, PIC_EOI);
    }

    outb(PIC1, PIC_EOI);
}

static void RemapIRQ(){
    uint8 mask1 = inb(PIC1_DATA);
    uint8 mask2 = inb(PIC2_DATA);
    outb(PIC1, ICW1_INIT | ICW1_ICW4);
    outb(PIC2, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, PIC1_OFFSET);
    outb(PIC2_DATA, PIC2_OFFSET);
    outb(PIC1_DATA, 0x04); // PIC2 at IRQ2
    outb(PIC2_DATA, 0x02); // Cascade indentity
    outb(PIC1_DATA, PIC_MODE_8086);
    outb(PIC1_DATA, PIC_MODE_8086);
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

static void SetIRQMask(size_t i){
    uint16 port = i < 8 ? PIC1_DATA : PIC2_DATA;
    uint8 value = inb(port) | 1 << i;
    outb(port, value);
}

static void ClearIRQMask(size_t i){
    uint16 port = i < 8 ? PIC1_DATA : PIC2_DATA;
    uint8 value = inb(port) & ~(1 << i);
    outb(port, value);
}

void InstallIRQ(size_t i, void (*handler)(struct Registers*)){
    cli;
    handlers[i] = handler;
    ClearIRQMask(i);
    sti;
}

void InitIRQ(){
    RemapIRQ();

    for(size_t i = 0; i < 16; i++){
        InstallISR(32 + i, stub);
    }
}