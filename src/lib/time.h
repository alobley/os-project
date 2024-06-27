#ifndef TIME_H
#define TIME_H

#define PIT_FREQUENCY 1193182

// Initialize the PIT at a specific frequency
void InitializePIT();

// Reads the PIT status on channel 0 and returns 1 if a tick has occurred, 0 otherwise.
unsigned short read_pit();

// Delay for a specified amount of ms (or ticks if the PIT is not set to milliseconds)
void delay(unsigned int ms);

#endif