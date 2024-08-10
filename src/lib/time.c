#include "time.h"
#include "types.h"
#include "io.h"
#include "asm.h"
#include "drivers/vga.h"
#include "interrupts/idt.h"
#include "interrupts/interrupts.h"
#include "interrupts/pic.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

uint32 pitTicks = 0;

extern void pit_interrupt();

// Initialize the PIT at a specific frequency
void InitializePIT(){
    uint16 hz = 1000;
    uint32 divisor = PIT_FREQUENCY / hz;

    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, (uint8)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void timer_callback(){
    pitTicks++;
}

// Delays execution for the specified number of milliseconds
void delay(unsigned int ms){
    uint32 startTicks = pitTicks;

    uint32 target = startTicks + ms;

    while(pitTicks < target){
        // Force the compiler not to optimize this
        volatile uint8 j = 0;
        kprintf("%d\n", pitTicks);
    }
}
