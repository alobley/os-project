#ifndef TIME_H
#define TIME_H

#define PIT_FREQUENCY 1193182

// Initialize the PIT at a specific frequency
void InitializePIT();

// Delay for a specified amount of ms (or ticks if the PIT is not set to milliseconds)
void delay(unsigned int ms);

#endif