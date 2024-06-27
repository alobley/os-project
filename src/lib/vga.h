#ifndef VGA_H
#define VGA_H

// Deprecated. Can be used for debugging purposes if needed.

#include "types.h"

#define VGA_TEXT_MODE_START 0xB8000
#define TEXT_MODE_WIDTH 80
#define TEXT_MODE_HEIGHT 25

static inline void WriteStr(const char* str, int x, int y){
    uint16* fb = (uint8* )VGA_TEXT_MODE_START;
    int i = 0;
    while(*str != "\0"){
        fb[i + x + (y * TEXT_MODE_WIDTH)] = str[i] << 8 | 0xF0;
    }
}

#endif