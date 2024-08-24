#include "keyboard.h"
#include "../io.h"
#include "../interrupts/irq.h"

#include "console.h"

uint8 scancode = 0;

#pragma GCC push_options
#pragma GCC optimize("O0")
void keyboard_handler(struct Registers *regs){
    scancode = inb(KBD_DATA_PORT);
    outb(0x20, PIC_EOI);
}

void InitializeKeyboard(){
    scancode = 0;
    InstallIRQ(KB_IRQ, keyboard_handler);
}

uint8 WaitForKeyPress(){
    uint8 currentKey = 0;
    while(currentKey == 0){
        currentKey = GetKey();
    }

    return currentKey;
}

// Get a PS/2 scancode
uint8 GetKey(){
    return scancode;
}

#pragma GCC pop_options