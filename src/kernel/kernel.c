#include <isr.h>
#include <irq.h>
#include <idt.h>
#include <vga.h>
#include <keyboard.h>
#include <alloc.h>
#include <time.h>
#include <fpu.h>
#include <pcspkr.h>

// To update:
// Do git add [filename], or git add .
// Do git commit -m "Say what you did"
// To push, do git push -u origin main

// Reference a small example for showcase
extern void LittleGame();

bool strcmp(const char* in1, const char* in2){
    for(int i = 0; i < strlen(in1); i++){
        if(in2[i] != in1[i]){
            // They are not the same
            return false;
        }
    }

    // They are the same
    return true;
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
    }else{
        printk("Invalid Command!\n");
    }
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
void kernel_main(){
    InitializeHardware();
    printk("Thanks for the GRUB!\n");
    printk("Enter \"game\" into the console to play a game!\n");

    //while(!IsKeyPressed(G) && !IsKeyPressed(ESC));

    if(IsKeyPressed(G)){
        LittleGame();
    }

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

    for(;;) asm("hlt");
}