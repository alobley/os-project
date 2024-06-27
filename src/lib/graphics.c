#include "graphics.h"
#include "types.h"

// Constants that define GPU commands
#define VIRTIO_GPU_CMD_SET_SCANOUT 0x0100
#define VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING 0x0200
#define VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D 0x0300

// virtio GPU control struct
struct virtio_gpu_ctrl_hdr{
    uint32 type;
    uint32 flags;
    uint64 fence_id;
    uint32 ctx_id;
    uint32 padding;
};

// Virtio GPU Display config struct
struct virtio_gpu_resp_display_info{
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_display_one{
        struct virtio_gpu_rect{
            uint32 x, y, width, height;
        } r;
        uint32 enabled;
        uint32 flags;
    }pmodes[16];
};

// Sets the resolution of the display by directly communicating with the GPU
void virtio_gpu_set_resolution(uint32 width, uint32 height, virtio_regs* GPU, virtq* queue){
    // Create and define the GPU data instructions
    struct virtio_gpu_ctrl_hdr cmd = {
        .type = VIRTIO_GPU_CMD_SET_SCANOUT,
        .flags = 0,
        .fence_id = 0,
        .ctx_id = 0,
        .padding = 0,
    };

    // This is the actual data that will be passed to the GPU queue
    struct virtio_gpu_set_scanout {
        struct virtio_gpu_ctrl_hdr hdr;
        struct virtio_gpu_rect {
            uint32 x;
            uint32 y;
            uint32 width;
            uint32 height;
        } r;
        uint32 scanout_id;
        uint32 padding;
    } scanout_cmd = {
        .hdr = cmd,
        .r = { .x = 0, .y = 0, .width = width, .height = height },
        .scanout_id = 0,
        .padding = 0
    };

    // Add the set resolution instruction to the queue
    virtio_enqueue(GPU, queue, &scanout_cmd, sizeof(scanout_cmd));
    GPU->QueueNotify = 0;
}