#include "lib/types.h"
#include "lib/virtio.h"
#include "lib/io.h"
#include "lib/graphics.h"
#include "lib/memory.h"
#include "lib/keyboard.h"
#include "lib/time.h"
#include "lib/vga.h"
#include "lib/asm.h"

// For some reason, even when it's not called at all, the interrupt code simply existing in memory causes a double fault. When I don't compile the interrupt stuff
// It works without any problems whatsoever.
#include "lib/interrupts/interrupts.h"

extern char __bss_start, __bss_end;


// The main function of the kernel, although the entry point is in kernel_start.s
void kernel_main(void){
    // Set the bss section to 0
    uint8* ptr = &__bss_start;
    while(ptr != &__bss_end){
        *ptr = 0;
        ptr++;
    }

    IntrInit();


    // Set rax to 0xDEADBEEF to conform that the kernel loaded
    asm volatile ("mov $0xDEADBEEF, %rax");

    ClearVGAMem();

    // Test interrupt
    asm volatile ("int $0x3");

    delay(1);

    WriteStr("Hello, World!", 0, 0);

    // Initialize the kernel's memory
    init_memory();

    // Find the GPU's MMIO address
    uint64 gpuaddr = find_virtio_device(GPU_VENDORID, GPU_DEVICEID);
    if(gpuaddr == DRIVER_FAILURE){
        PANIC_KERNEL_ERROR();
    }

    WriteStr("GPU Found!", 0, 0);

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