#include "keyboard.h"
#include "io.h"
#include <isr.h>
#include <idt.h>
#include <irq.h>
#include <vga.h>

#define KEYBOARD_ISR 0x21
#define KEYBOARD_IRQ 1

static byte keysDown[256];

void WaitForKeyPress(){
    uint8 currentKey = 0;
    while(currentKey == 0){
        currentKey = GetKey();
    }
}

void kb_handler(){
    uint8 scanCode = inb(KBD_DATA_PORT);
    
    if(scanCode & EVENT_KEYUP){
        keysDown[scanCode ^ EVENT_KEYUP] = false;
    }else{
        keysDown[scanCode] = true;
    }

    outb(PIC_EOI, PIC_EOI);
}

// Get a PS/2 scancode (DEPRECATED)
uint8 GetKey(){
    // Get the keyboard scancode
    uint8 scanCode = inb(KBD_DATA_PORT);

    return scanCode;
}

bool IsKeyPressed(uint8 scanCode){
    if(keysDown[scanCode]){
        return true;
    }else{
        return false;
    }
}

void InitializeKeyboard(){
    InstallISR(KEYBOARD_ISR, kb_handler);
    InstallIRQ(KEYBOARD_IRQ, kb_handler);
}