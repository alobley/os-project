#include "keyboard.h"
#include "io.h"

// Get a PS/2 scancode
uint8 GetKey(){
    // Get the keyboard scancode
    uint8 scanCode = inb(KBD_DATA_PORT);

    return scanCode;
}