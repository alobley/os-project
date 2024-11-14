#include "keyboard.h"
#include "io.h"
#include <isr.h>
#include <idt.h>
#include <irq.h>
#include <vga.h>
#include <time.h>

#define KEYBOARD_ISR 0x21
#define KEYBOARD_IRQ 1

static bool keysDown[256];
static uint8 lastKeyPressed = 0;

void WaitForKeyPress(){
    uint8 currentKey = 0;
    while(currentKey == 0){
        currentKey = GetKey();
    }
}

void WaitForRelease(uint8 ScanCode){
    while(IsKeyPressed(ScanCode));
}

bool shiftPressed = false;


void kb_handler(){
    uint8 scanCode = inb(KBD_DATA_PORT);

    if(scanCode & EVENT_KEYUP){
        // On key release
        if((scanCode ^ EVENT_KEYUP) == LSHIFT || (scanCode ^ EVENT_KEYUP) == RSHIFT){
            // Detect a shift release
            shiftPressed = false;
        }
        keysDown[scanCode ^ EVENT_KEYUP] = false;
        return;
    }

    if(scanCode == LSHIFT || scanCode == RSHIFT){
        shiftPressed = true;
    }
    
    if(keyASCII[scanCode] != 0){
        if(shiftPressed){
            keysDown[scanCode] = true;
            lastKeyPressed = ASCIIUpper[scanCode];
        }else{
            keysDown[scanCode] = true;
            lastKeyPressed = keyASCII[scanCode];
        }
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

uint8 GetLastKey(){
    uint8 key = lastKeyPressed;
    lastKeyPressed = 0;
    return key;
}

void InitializeKeyboard(){
    InstallISR(KEYBOARD_ISR, kb_handler);
    InstallIRQ(KEYBOARD_IRQ, kb_handler);
}