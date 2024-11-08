#include "lib/types.h"
#include "lib/memory.h"
#include "lib/asm.h"
#include "lib/time.h"
#include "lib/fpu.h"
#include "lib/math.h"
#include "lib/multiboot.h"
#include "lib/interrupts/idt.h"
#include "lib/interrupts/irq.h"
#include "lib/interrupts/isr.h"
#include "lib/drivers/ata.h"
#include "lib/drivers/console.h"
#include "lib/drivers/keyboard.h"
#include "lib/drivers/pci.h"
#include "lib/drivers/graphics.h"
#include "lib/drivers/fat32.h"

// This was to test adding functions to the timer. Confirmed proper working order.
void ktimer(){
    kprintf("Extra timer functions are working properly!\n");
}

void InitializeHardware(){
    InitializeMemory();

    InitIDT();
    InitISR();
    InitializeFPU();
    InitIRQ();

    InitializePIT();
    InitializeKeyboard();
}


void ChangeVolumeLabel(disk_info_t* disk, BIOS_parameter_block_t* header){
    if(disk == NULL || disk->CurrentMode != LBA_48_BIT){
        kprintf("Disk is currently unsupported.\n");
        return;
    }

    uint16 sectorCount = 1;
    lba_offset_t lbaOffset = FAT32_BOOT_SECTOR_OFFSET;

    volatile char newName[11] = "TEST-DRIVE\0";

    for(int i = 0; i < 11; i++){
        header->volumeLabel[i] = newName[i];
    }

    WriteSectors(disk, lbaOffset, sectorCount, (uint16* )header);
}

void DefineHardDisk(disk_info_t* disk){
    if(disk == NULL){
        return;
    }
    
    // See if what we did works by reading the FAT32 header from a given disk
    BIOS_parameter_block_t* header = ReadFat32Header(disk);
    fat_fs_t* fs = DefineFileSystem(header, disk);

    if(header->bootSig == 0xAA55){
        kprintf("Boot code: %x\n");
        kprintf("OEM Name: %s\n", header->oemName);
    }

    kfree(header);

    if(fs == NULL){
        kprintf("There was an error!\n");
        return;
    }

    if(fs->fsType != FS_FAT32){
        kprintf("The disk is not formatted to FAT32!\n");
        return;
    }

    kprintf("Fs type: %d (FAT32)\n", fs->fsType);

    kprintf("Disk size: %d\n", fs->totalSectors * FAT32_SECTOR_SIZE);

    kprintf("First sector containing a FAT: %d\n", fs->firstFatSector);

    GetCompatibility(disk);

    ParseRoot(fs, disk);

    return;

    int (*FileToCall)() = (int (*)())LoadFile(fs, disk, (void *)300000000, "PRGM.BIN");

    uint32 hi = *((uint32* )*FileToCall);

    if(hi == 0){
        kprintf("The file could not be loaded!\n");
        return;
    }

    kprintf("Value at loaded memory location: 0x%x\n", hi);

    uint32 the = FileToCall();
    kprintf("Program returned: %llu\n", the);

    //ChangeVolumeLabel(disk, header);
}

void kernel_main(){
    InitializeHardware();

    ClearTerminal();

    kprintf("Booting...\n");

    Sleep(1000);

    kprintf("Timer is working!!\n");

    kprintf("Keyboard test. Press any key to continue.\n");

    uint8 key = 0;
    key = WaitForKeyPress();

    kprintf("Keyboard is working!\n");

    kprintf("Kernel Loaded Without Errors.\n\n");

    // Create an array of pointers to the allocated info of the drives
    disk_info_t* hardDisks[MAX_SUPPORTED_DRIVES];

    // Search the ATA buses for disks
    for(int i = 0; i <= MAX_SUPPORTED_DRIVES; i++){
        hardDisks[i] = InitializeDisk(i);
    }

    // Wait untul the user presses enter to scan for ATA-compatible hard drives
    key = 0;
    kprintf("Press Enter to scan for disks.\n");
    while(key != ENTER){
        key = WaitForKeyPress();
    }

    kprintf("Scanning, please wait...\n");

    // Clear the terminal to show all the device outputs at once
    ClearTerminal();

    for(int j = 0; j < MAX_SUPPORTED_DRIVES; j++){
        disk_info_t* deviceToUse = hardDisks[j];
        ScanDrive(deviceToUse);
        ScanSupportedModes(deviceToUse);
    }

    ClearTerminal();

    DefineHardDisk(hardDisks[1]);

    // Main loop will be implemented later
    for(;;) hlt();
}