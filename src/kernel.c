#include "lib/types.h"
#include "lib/virtio.h"
#include "lib/io.h"
#include "lib/graphics.h"
#include "lib/memory.h"
#include "lib/keyboard.h"
#include "lib/time.h"
#include "lib/vga.h"
#include "lib/asm.h"

// For some reason, the mere existence of interrupts in RAM causes very strange errors. I can not fathom why on any level.
// It was also working before, but for some reason, even though absolutely nothing changed, it's not working anymore.
//#include "lib/interrupts/interrupts.h"

extern char __bss_start, __bss_end;


// RIP and PC just freeze and do not continue after a certain point. Why? Hell if I know. It must be a QEMU bug or something.
void kernel_main(void){
    // Initialize the kernel's memory - must be done first
    init_memory();

    // Set the bss section to 0
    uint8* ptr = &__bss_start;
    while((uint64)ptr != (uint64)&__bss_end){
        *ptr = 0;
        ptr++;
    }

    ClearVGAMem();

    kprintf("Booting OS...\n");

    kprintf("Keyboard test. Please press any key.\n");

    // PS/2 keyboard drivers are working!
    volatile uint8 key = 0x00;

    // Hey compiler, this code exists by the way
    while(key == NONE || key == 250){
        key = (volatile uint8)GetKey();
    }

    kprintf("Keys are Working!\n");

    // Busy wait instead of using the PIT
    delay2(ROUGHLY_ONE_MILLISECOND);

    kprintf("Timing is functional!\n");

    // Find the GPU's MMIO address
    uint64 gpuaddr = find_virtio_device(GPU_VENDORID, GPU_DEVICEID);
    if(gpuaddr == DRIVER_FAILURE){
        kprintf("No vGPU found! Are you running on real hardware?\n");
        PANIC_KERNEL_ERROR();
    }

    kprintf("GPU Drivers Functional!\n");

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
