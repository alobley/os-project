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
extern void reboot();
extern void shutdown();

void syscall(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx){
    asm volatile("movl %0, %%edx" :: "Nd" (edx));
    asm volatile("movl %0, %%ecx" :: "Nd" (ecx));
    asm volatile("movl %0, %%ebx" :: "Nd" (ebx));
    asm volatile("movl %0, %%eax" :: "Nd" (eax));
    asm volatile("int %0" :: "Nd" (SYSCALL_INT));
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
    }else if(strcmp(cmd, "systest")){
        syscall(1, 2, 3, 4);
    }else if(strcmp(cmd, "help")){
        printk("game: runs a small game\nhi: say hello!\nreboot: reboots the machine\nshutdown: shuts down the computer (QEMU/Bochs only)\nsystest: execute a system call\nhelp: view this screen\n");
    }else{
        printk("Invalid Command!\n");
    }
}

int CliHandler(){
    printk("Thanks for the GRUB!\n");
    printk("Kernel-Integrated Shell (KISh)\n");
    printk("Enter \"help\" into the console for a list of commands.\n");
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
                    if(GetX() >= 5){
                        index--;
                        command[index] = 0;
                        WriteStrSize(&lastKey, 1);
                    }
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