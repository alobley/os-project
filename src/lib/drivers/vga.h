#ifndef VGA_H
#define VGA_H

// Primarily for kernel debugging by writing to the standard VGA

#include "../types.h"

#define VGA_TEXT_MODE_START 0xB8000ULL
#define TEXT_MODE_WIDTH 80
#define TEXT_MODE_HEIGHT 25

void ClearTerminal();

uint32 strlen(const char* string);

void kprintf(const char *str, ...);

void WriteStr(const char* str);

void MoveCursor(uint16 x, uint16 y);

#endif