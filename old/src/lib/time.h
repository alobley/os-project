#ifndef TIME_H
#define TIME_H

#include "types.h"

// Milliseconds
#define TIMER_TPS 1000

uint64 GetTicks();
void InitializePIT();
void Sleep(uint64 ms);
void AddTimerCallback(void callback(), uint32 callbackNum, uint32 interval);
void RemoveTimerCallback(uint32 callbackNum);

#endif