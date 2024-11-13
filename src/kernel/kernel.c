#include <isr.h>
#include <irq.h>
#include <idt.h>
#include <vga.h>
#include <keyboard.h>
#include <alloc.h>
#include <time.h>
#include <fpu.h>
#include <pcspkr.h>

// To update:
// If a new file was added, do git add [filename]
// To push, do git push -u origin main

// Reference a small example for showcase
extern void LittleGame();

// Initializes all the required components
void InitializeHardware(){
    InitializeMemory();
    InitIDT();
    InitISR();
    InitializeFPU();
    InitIRQ();
    InitializePIT();
    InitializeKeyboard();
}

// The kernel's main function
void kernel_main(){
    InitializeHardware();
    printk("Thanks for the GRUB!\n");
    printk("Press G to play a little game.\n");
    printk("Press ESC to skip.\n");

    while(!IsKeyPressed(G) && !IsKeyPressed(ESC));

    if(IsKeyPressed(G)){
        LittleGame();
    }

    printk("/ > ");

    for(;;) asm("hlt");
}