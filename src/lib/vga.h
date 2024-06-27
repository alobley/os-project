#ifndef VGA_H
#define VGA_H

// Primarily for kernel debugging by writing to the standard VGA

#include "types.h"

#define VGA_TEXT_MODE_START 0xB8000
#define TEXT_MODE_WIDTH 80
#define TEXT_MODE_HEIGHT 25

void ClearVGAMem();

void WriteStr(const char* str, int x, int y);

#endif