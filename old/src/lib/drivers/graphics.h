#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../types.h"

// Resolution and color constants
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BYTES_PER_PIXEL 4

void svga_set_mode(uint32 mode);
uint32 svga_get_caps();
void* svga_get_fb();

#endif