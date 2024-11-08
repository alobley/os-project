#include <isr.h>
#include <irq.h>
#include <idt.h>
#include <vga.h>
#include <keyboard.h>
#include <alloc.h>
#include <time.h>
#include <fpu.h>


void kernel_main(){
    InitIDT();
    InitISR();
    InitializeFPU();
    InitIRQ();
    InitializePIT();

    printk("Booted successfully!\n");
    printk("Press G to test graphics mode\n");
    printk("Press S to skip test: ");

    uint8 key = 0;
    while(key != G && key != S){
        key = GetKey();
    }

    if(key == G){
        VGA_SetMode(VGA_MODE_GRAPHICS);
        VGA_SetColorRGB8();     // For pixel mode
        uint8* fb = (uint8*)VGA_PIXEL_MODE_START;
        uint8 color = 0x00;
        for(int i = 0; i < VGA_PIXEL_MODE_SIZE; i++){
            *(fb + i) = color;
            color++;
        }
        Sleep(2000);
        VGA_SetMode(VGA_MODE_TEXT);
        printk("Test successful!\n");
    }

    printk("Yippee!\n");

    for(;;) asm("hlt");
}