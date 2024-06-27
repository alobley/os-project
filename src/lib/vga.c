#include "vga.h"

void ClearVGAMem(){
    for(int i = 0; i < 80 * 25; i++){
        *((uint16* )(VGA_TEXT_MODE_START + i)) = 0;
    }
}

void WriteStr(const char* str, int x, int y){
    uint16* fb = (uint16* )VGA_TEXT_MODE_START;
    int i = 0;
    while(*(str + i) != '\0'){
        fb[i + x + (y * TEXT_MODE_WIDTH)] = (0x0F << 8) | str[i];
        i++;
    }
}