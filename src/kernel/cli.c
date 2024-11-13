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

// Temporary CLI shell built into the kernel untill I get filesystem and ABI support

extern void LittleGame();

// 8042 reset
void reboot(){
    uint8 good = 0x02;
    while(good & 0x02){
        good = inb(0x64);
    }
    outb(0x64, 0xFE);
}

// QEMU-specific
void shutdown(){
    outw(0x604, 0x2000);
}

void ProcessCommand(const char* cmd){
    if(strlen(cmd) == 0){
        return;
    }

    if(strcmp(cmd, "game")){
        LittleGame();
    }else if(strcmp(cmd, "clear")){
        ClearTerminal();
    }else if(strcmp(cmd, "hi")){
        printk("Hello!\n");
    }else if(strcmp(cmd, "reboot")){
        reboot();
    }else if(strcmp(cmd, "shutdown")){
        shutdown();
    }else{
        printk("Invalid Command!\n");
    }
}

int CliHandler(){
    printk("Thanks for the GRUB!\n");
    printk("Enter \"game\" into the console to play a game!\n");
    // Allocate 1000 bytes for a command. That means a max of 1000 characters. Should be more than enough.
    char* command = (char*)alloc(1000);
    memset(command, 0, 1000);

    int index = 0;

    printk("/ > ");
    while(true){
        uint8 lastKey = GetLastKey();
        if(lastKey != 0){
            switch (lastKey)
            {
                case '\b':
                    index--;
                    command[index] = 0;
                    WriteStrSize(&lastKey, 1);
                    break;

                case '\n':
                    printk("\n");
                    ProcessCommand(command);
                    memset(command, 0, 1000);
                    index = 0;
                    printk("/ > ");
                    break;
                
                default:
                    command[index] = lastKey;
                    index++;
                    WriteStrSize(&lastKey, 1);
                    break;
            }
        }
    }

    dealloc(command);

    return 1;
}