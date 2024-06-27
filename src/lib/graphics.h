#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "virtio.h"

// Resolution and color constants
#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 720
#define PIXEL_SIZE 4
#define PIXEL_DEPTH 32

// The vendor and device ID of the GPU
#define GPU_VENDORID 0x1AF4
#define GPU_DEVICEID 0x1050

// Set the resolution of the GPU
void virtio_gpu_set_resolution(uint32 width, uint32 height, virtio_regs* GPU, virtq* queue);


#endif