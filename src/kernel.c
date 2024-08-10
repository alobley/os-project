#include "lib/types.h"
#include "lib/drivers/vga.h"
#include "lib/asm.h"
#include "lib/memory.h"
#include "lib/time.h"
#include "lib/interrupts/interrupts.h"
#include "lib/drivers/keyboard.h"
#include "lib/drivers/graphics.h"
#include "lib/drivers/virtio.h"
#include "lib/drivers/pci.h"
#include "lib/paging.h"

void kernel_main(void){
    init_memory();

    IntrInit();

    ClearTerminal();

    kprintf("Kernel Loaded Without Errors.\n");

    uint8 key = 0;

    kprintf("Keyboard test. Press any key to continue.\n");

    key = WaitForKeyPress();

    kprintf("Keyboard is working!\n");

    //delay(10);

    //kprintf("Time is working!\n");

    // Find the GPU's MMIO address
    uint64 gpuaddr = find_virtio_device(GPU_VENDORID, GPU_DEVICEID);
    if(gpuaddr == DRIVER_FAILURE){
        kprintf("No vGPU found! Are you running on real hardware?\n");
        PANIC_KERNEL_ERROR();
    }

    kprintf("GPU Found!\n");

    //virtio_regs* GPU = set_virtio_device(gpuaddr);
    //if(virtio_device_init(GPU) == -1){
        //PANIC_KERNEL_ERROR();
    //}

    //virtq gpuQueue = setup_virtqueue(GPU);
    //virtio_gpu_set_resolution(1080, 720, GPU, &gpuQueue);

    //kprintf("No faults! Did the kernel initialize the  GPU?\n");

    // Main loop will be implemented later
    for(;;){
        hlt();
    }
}