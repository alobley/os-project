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
#include <ata.h>

// Very simple CLI shell built into the kernel until I get filesystem and ABI support

extern void LittleGame();
extern void reboot();
extern void shutdown();

// Execute a syscall to see what happens
void syscall(uint32 eax, uint32 ebx, uint32 ecx, uint32 edx){
    asm volatile("movl %0, %%edx" :: "Nd" (edx));
    asm volatile("movl %0, %%ecx" :: "Nd" (ecx));
    asm volatile("movl %0, %%ebx" :: "Nd" (ebx));
    asm volatile("movl %0, %%eax" :: "Nd" (eax));
    asm volatile("int %0" :: "Nd" (SYSCALL_INT));
}

// The shell commands
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
        printk("game: runs a small game\n");
        printk("hi: say hello!\n");
        printk("systest: execute a system call\n");
        printk("help: view this screen\n");
        printk("dskchk: scans the system for PATA disks\n");
        printk("clear: clears the terminal screen\n");
        printk("reboot: reboots the machine\n");
        printk("shutdown: shuts down the computer (QEMU/Bochs only)\n");
    }else if(strcmp(cmd, "dskchk")){
        disk_t* disks[MAX_DRIVES];
        for(int i = 0; i < MAX_DRIVES; i++){
            disks[i] = IdentifyDisk(i);
            if(disks[i] != NULL){
                printk("Disk found! Number: %d\n", disks[i]->driveNum);
                printk("Disk Type: %d ", disks[i]->type);
                if(disks[i]->type == PATADISK){
                    printk("(PATA)\n");
                }else if(disks[i]->type == PATAPIDISK){
                    printk("(PATAPI)\n");
                }else{
                    printk("(UNKNOWN)\n");
                }
                printk("Disk size in sectors: %llu\n", disks[i]->size);
                printk("Addressing: %d ", disks[i]->addressing);
                if(disks[i]->addressing == CHS_ONLY){
                    printk("(CHS only)\n");
                }else if(disks[i]->type == LBA28){
                    printk("(28-bit LBA)\n");
                }else{
                    printk("(48-bit LBA)\n");
                }
            }
        }
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

    printk("KISh> ");
    while(true){
        uint8 lastKey = GetKey();
        if(lastKey != 0){
            switch (lastKey)
            {
                case '\b':
                    if(GetX() > 6){
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
                    printk("KISh> ");
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