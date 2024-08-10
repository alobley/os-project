#include "keyboard.h"
#include "../io.h"

#include "vga.h"

uint8 WaitForKeyPress(){
    uint8 currentKey = 0;
    while(currentKey == 0 || currentKey == 250){
        currentKey = GetKey();
    }

    return currentKey;
}

// Get a PS/2 scancode
uint8 GetKey(){
    // Get the keyboard scancode
    uint8 scanCode = inb(KBD_DATA_PORT);

    return scanCode;
}