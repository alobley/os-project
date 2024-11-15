#include "ata.h"
#include <io.h>
#include <util.h>
#include <alloc.h>
#include <vga.h>

#define NUM_BUS_PORTS 7

// Drives 0 and 1
#define PRIMARY_BUS_BASE 0x1F0
#define PRIMARY_BUS_CTRL 0x3F6

// Drives 2 and 3
#define SECONDARY_BUS_BASE 0x170
#define SECONDARY_BUS_CTRL 0x376

// Drives 4 and 5
#define TERTIARY_BUS_BASE 0x1E8
#define TERTIARY_BUS_CTRL 0x3E6

// Drives 6 and 7
#define QUATERNARY_BUS_BASE 0x168
#define QUATERNARY_BUS_CTRL 0x366

// ATA commands
#define COMMAND_FLUSH_CACHE 0xE7
#define COMMAND_IDENTIFY 0xEC

#define MASTER_DRIVE 0xA0
#define SLAVE_DRIVE 0xB0

#define FLOATING_BUS 0xFF           // If the regular status byte contains this, there are no drives on this bus

// These macros get ATA ports based on a given base
#define DataPort(basePort) basePort             // The data port is the base port, but this makes things more readable.
#define SectorCount(basePort) (basePort + 2)
#define LbaLo(basePort) (basePort + 3)
#define LbaMid(basePort) (basePort + 4)
#define LbaHi(basePort) (basePort + 5)
#define DriveSelect(basePort) (basePort + 6)    // Drive select port
#define CmdPort(basePort) (basePort + 7)        // Write-Only
#define StatusPort(basePort) (basePort + 7)     // Read-Only


// Get the control register of a given base
uint16 GetCtrl(uint16 basePort){
    switch (basePort){
        case PRIMARY_BUS_BASE:
            return PRIMARY_BUS_CTRL;
        case SECONDARY_BUS_BASE:
            return SECONDARY_BUS_CTRL;
        case TERTIARY_BUS_BASE:
            return TERTIARY_BUS_CTRL;
        case QUATERNARY_BUS_BASE:
            return QUATERNARY_BUS_CTRL;
        default:
            // Invalid base
            return 0;
    }
}

// Read from the control register 14 times after selecting a device. Read from it again to get #15 and use that data.
void DiskDelay(uint16 BasePort){
    for(int i = 0; i < 14; i++){
        inb(GetCtrl(BasePort));
    }
}

// Checks if the disk is running a command by checking the BSY bit
bool IsBusy(uint16 basePort){

}

// Wait for the BSY bit to clear so the next command can be sent
#pragma GCC push_options
#pragma GCC optimize("O0")
void WaitForIdle(uint16 basePort){
    while(inb(StatusPort(basePort)) & 0x80);
}

void WaitForDrq(uint16 basePort){
    while(inb(StatusPort(basePort)) & 0x08 != 0x08);
}
#pragma GCC pop_options

// Flush the disk cache, should be done after every write command
void CacheFlush(uint16 basePort){
    outb(basePort, COMMAND_FLUSH_CACHE);
}

void DetermineAddressing(disk_t* disk){
    // Default to CHS
    disk->addressing = CHS_ONLY;
    if(disk->infoSector[83] & (1 << 10)){
        // LBA48 supported
        disk->addressing = LBA48;
    }

    if(disk->addressing == CHS_ONLY){
        // If not 48-bit LBA, check for 28-bit LBA
        uint32 lba28Sectors = (disk->infoSector[60] << 16) | (disk->infoSector[61]);
        if(lba28Sectors > 0){
            disk->addressing = LBA28;
            disk->size = lba28Sectors;
        }else{
            // The disk is truly CHS only
            // Get the size of the disk from the number of cylinders, the number of heads, and the number of sectors per track, respectively.
            uint64 chsSectors = disk->infoSector[9] * disk->infoSector[11] * disk->infoSector[13];
            disk->size = chsSectors;
        }
    }else{
        // If 48-bit LBA, get the number of 48-bit LBA sectors
        uint64 lba48Sectors = (disk->infoSector[100] << 48) | (disk->infoSector[101] << 32) | (disk->infoSector[102] << 16) | (disk->infoSector[103]);
        if(lba48Sectors > 0){
            disk->size = lba48Sectors;
        }else{
            // No 48-bit LBA
            uint32 lba28Sectors = (disk->infoSector[60] << 16) | (disk->infoSector[61]);
            disk->size = lba28Sectors;
            disk->addressing = LBA28;
        }
    }
}


// PATAPI has NOT been implemented yet in this! It needs to be implemented at some point.
disk_t* IdentifyDisk(uint8 diskNum){
    disk_t* disk = (disk_t*)alloc(sizeof(disk_t));
    switch (diskNum){
        // Enter disk information based on the disk number provided
        case 0:
            disk->driveNum = diskNum;
            disk->slave = false;
            disk->base = PRIMARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        case 1:
            disk->driveNum = diskNum;
            disk->slave = true;
            disk->base = PRIMARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        case 2:
            disk->driveNum = diskNum;
            disk->slave = false;
            disk->base = SECONDARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        case 3:
            disk->driveNum = diskNum;
            disk->slave = true;
            disk->base = SECONDARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        case 4:
            disk->driveNum = diskNum;
            disk->slave = false;
            disk->base = TERTIARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        case 5:
            disk->driveNum = diskNum;
            disk->slave = true;
            disk->base = TERTIARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        case 6:
            disk->driveNum = diskNum;
            disk->slave = false;
            disk->base = QUATERNARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        case 7:
            disk->driveNum = diskNum;
            disk->slave = true;
            disk->base = QUATERNARY_BUS_BASE;
            disk->ctrl = GetCtrl(disk->base);
            break;
        default:
            // Invalid disk number
            dealloc(disk);
            return NULL;
    }

    if(inb(StatusPort(disk->base)) == FLOATING_BUS){
        // Floating bus, no disks in this bus
        dealloc(disk);
        return NULL;
    }

    // Select the master or slave depending on the drive
    if(disk->slave){
        outb(DriveSelect(disk->base), SLAVE_DRIVE);
    }else{
        outb(DriveSelect(disk->base), MASTER_DRIVE);
    }

    DiskDelay(disk->base);

    outb(SectorCount(disk->base), 0);
    outb(LbaLo(disk->base), 0);
    outb(LbaMid(disk->base), 0);
    outb(LbaHi(disk->base), 0);

    outb(CmdPort(disk->base), COMMAND_IDENTIFY);

    uint8 driveStatus = inb(StatusPort(disk->base));
    if(driveStatus == 0){
        // Drive does not exist
        dealloc(disk);
        return NULL;
    }else{
        if(inb(LbaMid(disk->base)) == 0x3C && inb(LbaHi(disk->base)) == 0xC3){
            // Drive is SATA, which is unsupported but the infrastructure should be here
            disk->type = SATADISK;
        }else{
            if(driveStatus & 0x01){
                // There was an error, the drive can't be used
                dealloc(disk);
                return NULL;
            }else{
                // Regular PATA disk. Used normally.
                WaitForIdle(disk->base);
                if(inb(LbaMid(disk->base)) == 0x14 && inb(LbaHi(disk->base)) == 0xEB){
                    // Drive is PATAPI
                    disk->type = PATAPIDISK;
                }else{
                    // We can finally confirm the existence of a PATA drive
                    disk->type = PATADISK;
                }
            }
        }
    }

    uint8 status = inb(StatusPort(disk->base));
    while(!(status & 0x08)){
        // Wait until DRQ is ready
        if(status & 0x01){
            // There was an error, the disk cannot be used
            dealloc(disk);
            return NULL;
        }
        status = inb(StatusPort(disk->base));
    }

    uint16* diskBuffer = (uint16*)alloc(512);
    disk->infoSector = diskBuffer;
    for(int i = 0; i < 256; i++){
        // Read the buffer into memory
        disk->infoSector[i] = inw(DataPort(disk->base));
    }

    DetermineAddressing(disk);
    return disk;
}