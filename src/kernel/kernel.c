#include <isr.h>
#include <irq.h>
#include <idt.h>
#include <vga.h>
#include <keyboard.h>
#include <alloc.h>
#include <time.h>
#include <fpu.h>
#include <pcspkr.h>
#include <string.h>

// To update:
// Do git add [filename], or git add .
// Do git commit -m "Say what you did"
// To push, do git push -u origin main

// Reference a small example for showcase
extern int CliHandler();

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

    // Launch the shell
    int value = CliHandler();

    // Temporary solution since the shell is built-in. We shouldn't return from it.
    printk("CRITICAL ERROR! Rebooting...\n");

    Sleep(1000);

    asm volatile("lidt 0");
    asm volatile("int $0x1");

    for(;;) asm("hlt");
}