#include "keyboard.h"
#include "io.h"

void WaitForKeyPress(){
    uint8 currentKey = 0;
    while(currentKey == 0){
        currentKey = GetKey();
    }
}

// Get a PS/2 scancode
uint8 GetKey(){
    // Get the keyboard scancode
    uint8 scanCode = inb(KBD_DATA_PORT);

    return scanCode;
}