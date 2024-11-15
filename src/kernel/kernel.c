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
#include <ata.h>

#define MULTIBOOT_MAGIC 0x2BADB002

// To update:
// Do git add [filename], or git add .
// Do git commit -m "Say what you did"
// To push, do git push -u origin main

// Reference a small example for showcase
extern int CliHandler();

// 8042 reset
void reboot(){
    uint8 good = 0x02;
    while(good & 0x02){
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
}

// QEMU and Bochs only. ACPI support pending.
void shutdown(){
    // Try QEMU shutdown
    outw(0x604, 0x2000);

    // Try Bochs shutdown
    outw(0xB004, 0x2000);
}

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
void kernel_main(uint32 magic, uintptr_t placeholder){
    if(magic != MULTIBOOT_MAGIC){
        // There was a problem, reboot
        printk("WARNING: no multiboot magic number.\n");
    }
    InitializeHardware();

    // Launch the shell
    int value = CliHandler();

    // Temporary solution since the shell is built-in. We shouldn't return from it.
    printk("CRITICAL ERROR! Rebooting...\n");

    Sleep(1000);

    reboot();

    for(;;) asm("hlt");
}