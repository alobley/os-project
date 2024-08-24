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

BIOS_parameter_block_t* ReadFat32Header(disk_info_t* disk){
    if(disk == NULL || disk->CurrentMode != LBA_48_BIT){
        kprintf("Disk is currently unsupported.\n");
        return NULL;
    }

    uint16 sectorCount = 1;
    lba_offset_t lbaOffset = FAT32_BOOT_SECTOR_OFFSET;

    uint8* buffer = ReadSectors(disk, lbaOffset, sectorCount);
    if(buffer == NULL){
        kprintf("Failed to read from the disk.\n");
        return NULL;
    }

    BIOS_parameter_block_t* header = buffer;

    kprintf("OEM name: ");
    kprintf(header->oemName);
    kprintf("\n");

    return header;
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

void GetCompatibility(disk_info_t* disk){
    if(disk->deviceType == DEVTYPE_ATA){
        kprintf("Device is ATA/PATA compatible!\n");
    }
}

void DefineHardDisk(disk_info_t* disk){
    ClearTerminal();
    // See if what we did works by reading the FAT32 header from a given disk
    BIOS_parameter_block_t* header = ReadFat32Header(disk);
    kprintf("Disk: %d\n", disk->diskID);
    kprintf("Volume name: ");
    WriteStrSize(header->volumeLabel, 11);
    kprintf("\n");

    kprintf("File system type: ");
    WriteStrSize(header->filesystemType, 8);
    kprintf("\n");

    kprintf("Size of disk %d in bytes: %llu\n", disk->diskID, disk->sizeInBytes);
    kprintf("Current disk mode: %d\n", disk->CurrentMode);

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
    for(int i = 0; i < MAX_SUPPORTED_DRIVES + 1; i++){
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
        //ScanDrive(deviceToUse);
        ScanSupportedModes(deviceToUse);
    }

    DefineHardDisk(hardDisks[1]);

    // Main loop will be implemented later
    for(;;) hlt();
}