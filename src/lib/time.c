#include "time.h"
#include "types.h"
#include "io.h"
#include "asm.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43

volatile uint32 pitTicks;

// Initialize the PIT at a specific frequency
void InitializePIT(){
    uint16 hz = 1000;
    uint32 divisor = PIT_FREQUENCY / hz;

    outb(PIT_COMMAND, 0x00 | 0x06 | 0x30 | 0x00);
    outb(0x00, divisor);
    outb(0x00, divisor >> 8);
}

// Reads the PIT status on channel 0 and returns 1 if a tick has occurred, 0 otherwise.
uint16 read_pit(){
    uint32 count = 0;

    outb(0x43, 0x00);

    count = inb(PIT_CHANNEL0);
    count |= inb(PIT_CHANNEL0) << 8;

    return count;
}

void set_pit_count(uint32 count){
    outb(PIT_CHANNEL0, count & 0xFF);
    outb(PIT_CHANNEL0, (count & 0xFF00) >> 8);
}

// Delays execution for the specified number of milliseconds
void delay(uint32 ms){
    uint32 now = pitTicks;
    ++ms;

    while(pitTicks - now < ms){
        ;
    }
}
