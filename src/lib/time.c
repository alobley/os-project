#include "time.h"
#include "types.h"
#include "io.h"
#include "asm.h"
#include "vga.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

volatile uint32 pitTicks = 0;

// Initialize the PIT at a specific frequency
void InitializePIT(){
    uint16 hz = 1000;
    uint32 divisor = PIT_FREQUENCY / hz;

    outb(PIT_COMMAND, 0x00 | 0x06 | 0x30 | 0x00);
    outb(0x00, divisor);
    outb(0x00, divisor >> 8);
}

void set_pit_count(uint32 count){
    outb(PIT_CHANNEL0, count & 0xFF);
    outb(PIT_CHANNEL0, (count & 0xFF00) >> 8);
}

void delay2(size_t timeWait){
    for(size_t i = 0; i < timeWait; i++){
        // Force the compiler not to optimize this function
        volatile uint8 j = 0;
    }
}

// Delays execution for the specified number of milliseconds
void delay(uint32 ms){
    uint32 startTicks = pitTicks;

    uint32 target = startTicks + ms;

    while(pitTicks < target);
}
