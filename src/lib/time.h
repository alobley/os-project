#ifndef TIME_H
#define TIME_H

#define PIT_FREQUENCY 1193182

#define ROUGHLY_THREE_SECONDS 1000000000
#define ROUGHLY_ONE_SECOND 333333333
#define ROUGHLY_ONE_MILLISECOND 333333

// Initialize the PIT at a specific frequency
void InitializePIT();

void delay2(unsigned long long timeWait);

// Delay for a specified amount of ms (or ticks if the PIT is not set to milliseconds)
void delay(unsigned int ms);

#endif