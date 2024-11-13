#ifndef PIXEL_H
#define PIXEL_H

#include "vga.h"

#define COLOR_WHITE 0xFF
#define COLOR_BLACK 0x00

typedef struct Square {
    uint16 x;
    uint16 y;
    uint16 width;
    uint16 height;
    color c;
} square_t;

void VGA_ClearScreen();

square_t* VGA_CreateSquare(uint16 x, uint16 y, uint16 width, uint16 height, color c);

void VGA_DrawSquare(square_t* square);

void VGA_DrawPixel(int x, int y, color c);

#endif