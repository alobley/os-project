#include "lib/types.h"
#include "lib/virtio.h"
#include "lib/io.h"
#include "lib/graphics.h"
#include "lib/memory.h"


// The main function of the kernel, although the entry point is in kernel_start.s
void kernel_main(void){
    // Set rax to 0xDEADBEEF to conform that the kernel loaded
    asm("mov $0xDEADBEEF, %rax");

    // Initialize the kernel's memory
    init_memory();

    // Find the GPU's MMIO address
    uint64 gpuaddr = find_virtio_device(GPU_VENDORID, GPU_DEVICEID);
    if(gpuaddr == DRIVER_FAILURE){
        PANIC_KERNEL_ERROR();
    }

    // Set the pointer to the virtio registers struct to the address the GPU's MMIO is located.
    virtio_regs* GPU = set_virtio_device(gpuaddr);

    // Initialize the GPU
    if(virtio_device_init(GPU) == -1){
        PANIC_KERNEL_ERROR();
    }

    // Setup the GPU's command queue
    virtq gpuQueue = setup_virtqueue(GPU);

    // Set the resolution to 720p
    virtio_gpu_set_resolution(1080, 720, GPU, &gpuQueue);

    // Main loop will be implemented after confirming drivers work
    for(;;);
}