#ifndef VGA_H
#define VGA_H

// Primarily for kernel debugging by writing to the standard VGA

#include "../types.h"

#define VGA_TEXT_MODE_START 0xB8000U
#define TEXT_MODE_WIDTH 80
#define TEXT_MODE_HEIGHT 25

void ClearTerminal();

uint32 strlen(const char* string);

void kprintf(const char *str, ...);

void WriteStr(const char* str);

void WriteStrSize(const char* str, size_t size);

void MoveCursor(uint16 x, uint16 y);

static inline void memset(void *dst, uint8 value, size_t n) {
    uint8 *d = dst;

    while (n-- > 0) {
        *d++ = value;
    }
}

#endif