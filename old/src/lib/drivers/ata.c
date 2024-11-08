#include "ata.h"
#include "../asm.h"
#include "../io.h"
#include "../time.h"
#include "../memory.h"
#include "../string.h"
#include "console.h"
#include "fat32.h"
//
// IDE driver
//
// Right now, all it can do is detect if a drive exists or not

// Most ports except for data ports are 8 bits wide

// ATA bus I/O ports. Base ports are also the data ports. The data ports are 16 bits wide.
#define PRIMARY_ATA_BASE   0x1F0        // Through 0x1F7
#define SECONDARY_ATA_BASE 0x170        // Through 0x177

#define PRIMARY_ATA_CTRL   0x3F6        // Primary ATA Control Register Port
#define SECONDARY_ATA_CTRL 0x376        // Secondary ATA Control Register Port

// Only if these next two exist
#define TERTIARY_ATA_BASE   0x1E8       // Through 0x1EF
#define QUATERNARY_ATA_BASE 0x168       // Through 0x16F

#define TERTIARY_ATA_CTRL   0x3E6       // Tertiary ATA Control Register Port
#define QUATERNARY_ATA_CTRL 0x376       // Quaternary ATA Control Register Port
// With all of these at 2 possible drives per ATA bus, that makes 8 possible total hard drives. 4 if there is only primary and secondary.



#define ATA_IDENTIFY_MASTER 0xA0                                // Master drive identify command
#define ATA_IDENTIFY_SLAVE  0xB0                                // Slave drive identify command

#define ALTERNATE_ATA_IDENTIFY_MASTER 0x00
#define ALTERNATE_ATA_IDENTIFY_SLAVE  0x10

#define ATA_DAR(ctrl_reg) (ctrl_reg + 1)


#define ATA_ID_COMMAND 0xEC                                     // IDENTIFY command

#define ATA_DRIVE_DATA(ata_base) (ata_base + 0)                 // Get a data register port from a given base port (it is the base port but this adds consistency)
#define ATA_DRIVE_ERROR(ata_base) (ata_base + 1)                // Get an error port from a given base port
#define ATA_DRIVE_SECTORCOUNT(ata_base) (ata_base + 2)          // Get a sector count port from a given base port
#define ATA_DRIVE_LO(ata_base) (ata_base + 3)                   // Get a low byte port from a given base port
#define ATA_DRIVE_MID(ata_base) (ata_base + 4)                  // Get a medium byte port from a given base port
#define ATA_DRIVE_HIGH(ata_base) (ata_base + 5)                 // Get a high byte port from a given base port
#define ATA_DRIVE_HEAD(ata_base) (ata_base + 6)                 // Get a drive/head register port from a given base port
#define ATA_DRIVE_CMD_STATUS(ata_base) (ata_base + 7)           // Get a command/status register port from a given base port

// To select a drive based on the ATA_IDENTIFY_MASTER and ATA_IDENTIFY_SLAVE constants, send the ID constant to the drive/head register.

// After sending an IDENTIFY command, the low and high ports will read as these values if the device is ATA compatible.
#define ATA_COMPATIBLE_LOW  0x00
#define ATA_COMPATIBLE_HIGH 0x00

#define ATAPI_COMPATIBLE_LOW 0x14
#define ATAPI_COMPATIBLE_HIGH 0xEB


// If the device returns SATA compatibility, the low and high ports will read as these values.
// SATA will be implemented in the future.
#define SATA_COMPATIBLE_LOW  0x3C
#define SATA_COMPATIBLE_HIGH 0xC3

#define SATAPI_COMPATIBLE_LOW 0x69
#define SATAPI_COMPATIBLE_HIGH 0x96


// This will be what the regular status byte will equal when the IDE hub is empty
#define ATA_FLOATING_BUS 0xFF           // The bus has zero drives

#define ATA_FLUSH_CACHE  0xE7           // The cache flush command, passed to the command register (make sure to wait for the busy flag to clear)

// Flags from the status register
#define FLG_BUSY (1 << 7)               // Busy
#define FLG_READY (1 << 6)              // Device ready
#define DEVICE_FAULT (1 << 5)           // Device fault
#define SEEK_COMPLETE (1 << 4)          // Seek complete
#define DATA_TRNSFR_REQ (1 << 3)        // Data transfer requested
#define CORRECTED (1 << 2)              // Data corrected
#define INDEX_MARK (1 << 1)             // Index mark
#define ERROR (1 << 0)                  // Error

// Flags from the error register
#define BAD_BLK (1 << 7)                // Bad block
#define UNCORR_ERR (1 << 6)             // Uncorrected error
#define MEDIA_CHANGED (1 << 5)          // Media changed
#define ID_NOT_FOUND (1 << 4)           // ID not found
#define CHANGE_REQ (1 << 3)             // Media change requested
#define ABORT (1 << 2)                  // Command aborted
#define TRK0_NF (1 << 1)                // Track 0 not found
#define ADDR_MRK_NF (1 << 0)            // Address mark not found


#define MAX_DISKS_PER_BUS 2             // The maximum amount of disks per bus

// This value can be anything you want, but best practice dictates -1
#define TIMEOUT -1

// Get the offsets for specific data values in the buffer the disk sent to the CPU
#define TOTAL_CYLINDERS_OFFSET 1
#define TOTAL_HEADS_OFFSET 3
#define SECTORS_PER_TRACK_OFFSET 6

#define LBA_28_BIT_SUPPORT_OFFSET 49
#define LBA_48_BIT_SUPPORT_OFFSET 83
#define PIO_MODE_SUPPORT_OFFSET 63

#define LBA_28_BIT_SUPPORTED (1 << 9)
#define LBA_48_BIT_SUPPORTED (1 << 10)
#define PIO_28_BIT_SUPPORTED (1 << 0)
#define PIO_48_BIT_SUPPORTED (1 << 1)



// Important 48-bit PIO macros
// Note that a sectorcount of 0 equals 65536 sectors or 32MB of disk.
#define PIO_48_MASTER 0x40
#define PIO_48_SLAVE (1 << 4)



#define GET_LBA6(lbaOffset) ((uint8)((lbaOffset >> 40) & 0xFF))
#define GET_LBA5(lbaOffset) ((uint8)((lbaOffset >> 32) & 0xFF))
#define GET_LBA4(lbaOffset) ((uint8)((lbaOffset >> 24) & 0xFF))
#define GET_LBA3(lbaOffset) ((uint8)((lbaOffset >> 16) & 0xFF))
#define GET_LBA2(lbaOffset) ((uint8)((lbaOffset >> 8) & 0xFF))
#define GET_LBA1(lbaOffset) ((uint8)((lbaOffset >> 0) & 0xFF))

#define READ_SECTORS_EXT 0x24

// Do all the same things for write sectors except send this command instead.
#define WRITE_SECTORS_EXT 0x34

#define LBA_MAGIC 0x40

// How to issue a command:
// 1. Poll status register until busy is false
// 2. Disable interrupts
// 3. Poll status register until ready is true
// 4. Issue the command by sending the opcode to the command register
// 5. Reenable interrupts

// Before sending data to the I/O ports, we must read the (regular) status byte. If we don't, it could lead to an incorrect floating bus read.
// Also, a non-0xFF value is not definitive. The identify command should be sent just in case.


// On the primary ATA bus, ports 0x1F2 through 0x1F5 should all be read/write

// Note - while this isn't required for now, if the disks aren't found, then we'll have to ennumerate the disk controllers on the PCI bus.
// almost every time, the disk controllers will be at the standard I/O ports, unless your firmware isn't your OEM's and it's non-standard.


// ^ In the same fasion, the identify command has been clearly standardized for all BIOSes to detect the existence of all ATA bus devices

// IMPORTANT - make sure to wait during the following conditions:
// - Before and after a read or write operation
// - Before and after flushing the cache

// Just in case, we will manually flush the cache after every read/write. This is done by sending 0xE7 to the appropriate command register, then
// waiting until the busy bit is clear.

// Bad sectors can be a thing on real hardware - that's important.
// Some examples:
// - Sectors that can't be written
// - Sectors that can't be read (permanent)
// - Sectors that can't be read (temporary)


#pragma GCC push_options
#pragma GCC optimize("O0")
// Wait until the drive is done with all its commands
int8 ATAWait(uint8 basePort, uint64 timeout){
    // Get the current system time
    uint64 currentTime = GetTicks();
    uint64 targetTime = currentTime + timeout;        // timeout

    // Wait for the device to be ready for a command (documentation recommended checking both of these registers)
    while(((inb(ATA_DRIVE_CMD_STATUS(basePort)) & FLG_BUSY) & DATA_TRNSFR_REQ) == (FLG_BUSY | DATA_TRNSFR_REQ)){
        if(GetTicks() >= targetTime){
            // Timeout, wait failed. Possible device error.
            return TIMEOUT;
        }
    }

    // Wait successful
    return 0;
}
#pragma GCC pop_options


// Get the device identifier based on the device ID. Note that this defines the bus that will be used.
uint16 GetBasePort(uint8 driveID){
    if(driveID > MAX_SUPPORTED_DRIVES){
        // Disk out of range!
        return 0xFFFF;
    }

    // Get the total amount of buses used
    uint8 controllerID = driveID / MAX_DISKS_PER_BUS;
    switch(controllerID){
        case 0:
            // Drive 0 and 1
            return PRIMARY_ATA_BASE;
        case 1:
            // Drive 2 and 3
            return SECONDARY_ATA_BASE;
        case 2:
            // Drive 4 and 5
            return TERTIARY_ATA_BASE;
        case 3:
            // Drive 6 and 7
            return QUATERNARY_ATA_BASE;
        default:
            // No driver support for this controller!
            return 0xFFFF;
    }
}


// Determine whether a given drive is a master or slave
uint8 MasterOrSlave(uint8 driveID){
    if(driveID > MAX_SUPPORTED_DRIVES){
        // No driver support!
        return 0xFF;
    }

    if(driveID % 2 == 0){
        // If the drive number is even, it is a master
        return ATA_IDENTIFY_MASTER;
    }else{
        // If the drive number is odd, it is a slave
        return ATA_IDENTIFY_SLAVE;
    }
}

// Get the supported modes of the disk
mode_support_t* GetModes(uint16* buffer){
    mode_support_t* supportedModes = kmalloc(sizeof(mode_support_t));

    if(buffer[LBA_28_BIT_SUPPORT_OFFSET] & LBA_28_BIT_SUPPORTED){
        supportedModes->lba28 = true;
    }

    if(buffer[LBA_48_BIT_SUPPORT_OFFSET] & LBA_48_BIT_SUPPORTED){
        supportedModes->lba48 = true;
    }

    if(buffer[PIO_MODE_SUPPORT_OFFSET] & PIO_28_BIT_SUPPORTED){
        supportedModes->pio28 = true;
    }

    if(buffer[PIO_MODE_SUPPORT_OFFSET] & PIO_48_BIT_SUPPORTED){
        supportedModes->pio48 = true;
    }

    if(buffer[TOTAL_CYLINDERS_OFFSET] && buffer[TOTAL_HEADS_OFFSET] && buffer[SECTORS_PER_TRACK_OFFSET]){
        supportedModes->chs = true;
    }

    return supportedModes;
}

// Get the size of a given disk
uint64 GetDiskSize(disk_info_t* disk){
    if(disk == NULL || disk->dataBuffer == NULL){
        return 0;       // Disk doesn't exist or drive is empty, so its size is zero.
    }

    uint16 totalCylinders = disk->dataBuffer[TOTAL_CYLINDERS_OFFSET];
    uint16 totalHeads = disk->dataBuffer[TOTAL_HEADS_OFFSET];
    uint16 sectorsPerTrack = disk->dataBuffer[SECTORS_PER_TRACK_OFFSET];

    uint64 totalSectors = (uint64)(totalCylinders * totalHeads * sectorsPerTrack);

    uint64 sizeInBytes = totalSectors * MBR_SECTOR_SIZE;

    return sizeInBytes;
}

#define DETECT_MASTER 0xA0
#define DETECT_SLAVE (DETECT_MASTER | (1 << 4))

uint32 DetectDeviceType(disk_info_t* diskInfo){
    ATAWait(diskInfo->basePort, 2000);
    //uint16 basePort = GetBasePort(diskInfo->diskID);
    uint8 masterSlave = MasterOrSlave(diskInfo->diskID);

    if(masterSlave == ATA_IDENTIFY_MASTER){
        // Select the master disk of the current bus
        outb(ATA_DRIVE_HEAD(diskInfo->basePort), DETECT_MASTER);
    }else if(masterSlave == ATA_IDENTIFY_SLAVE){
        // Select the slave disk of the current bus
        outb(ATA_DRIVE_HEAD(diskInfo->basePort), DETECT_SLAVE);
        for(int i = 0; i < 4; i++){
            inb(diskInfo->basePort);
        }

        uint8 cl = inb(ATA_DRIVE_LO(diskInfo->basePort));
        uint8 ch = inb(ATA_DRIVE_HIGH(diskInfo->basePort));

        if(cl == ATA_COMPATIBLE_LOW && ch == ATA_COMPATIBLE_HIGH) return DEVTYPE_ATA;
        if(cl == ATAPI_COMPATIBLE_LOW && ch == ATAPI_COMPATIBLE_HIGH) return DEVTYPE_ATAPI;
        if(cl == SATA_COMPATIBLE_LOW && ch == SATA_COMPATIBLE_HIGH) return DEVTYPE_SATA;
        if(cl == SATAPI_COMPATIBLE_LOW && ch == SATAPI_COMPATIBLE_HIGH) return DEVTYPE_SATAPI;
    }
    return DEVTYPE_NONE;
}

// How to identify/initialize a drive:
// 1. Write the master or slave identifier to the drive/head register (drive select port) to select the drive
// 2. Set the LBAlo, LBAmid, and LBAhi to 0
// 3. Send IDENTIFY command to the command IO port
// 4. Read the status port again. If the value of the register is 0, the drive does not exist.
// IMPORTANT - after sending an identify command, the device will send 256 word values. Read them and store them (just read the data port 256 times).

void SetMode(disk_info_t* diskInfo){
    if(diskInfo->supportedModes->lba48){
        // Preferred mode is 48-bit LBA
        diskInfo->CurrentMode = LBA_48_BIT;
    }else if(diskInfo->supportedModes->lba28){
        // If 48-bit LBA isn't supported, try 28-bit LBA
        diskInfo->CurrentMode = LBA_28_BIT;
    }else if(diskInfo->supportedModes->pio48){
        // If 28-bit LBA isn't supported, try 48-bit PIO mode.
        diskInfo->CurrentMode = PIO_48_BIT;
    }else if(diskInfo->supportedModes->pio28){
        // If 48-bit PIO is somehow not supported, try 28-bit PIO mode
        diskInfo->CurrentMode = PIO_28_BIT;
    }else if(diskInfo->supportedModes->chs){
        // Final fallback if nothing else is supported. This should never happen unless the disk is very old.
        diskInfo->CurrentMode = CHS_MODE;
    }
}

// Find a given hard disk in the ATA controller, identify it, and initialize it.
disk_info_t* InitializeDisk(uint8 deviceID){
    // Allocate a disk info type to store the information of this disk
    disk_info_t* diskInfo = kmalloc(sizeof(disk_info_t));

    // Set the default values for a missing disk
    diskInfo->diskID = deviceID;
    diskInfo->CurrentMode = NO_MODE;        // No mode because I haven't implemented setting that yet
    diskInfo->dataBuffer = NULL;
    diskInfo->supportedModes = NULL;
    diskInfo->sizeInBytes = 0;
    diskInfo->deviceType = 0;               // Set to 0 for now

    // Get the base port of the drive
    uint16 basePort = GetBasePort(deviceID);
    if(basePort == 0xFFFF){
        // Device out of range! There is no spot for it.
        return NULL;
    }
    diskInfo->basePort = basePort;

    // Get the master or slave ID
    uint8 masterOrSlave = MasterOrSlave(deviceID);
    if(masterOrSlave == 0xFF){
        // Device out of range! (Just in case the last one didn't work)
        return diskInfo;
    }

    // Allocate a 512 byte buffer for the disk data
    uint16* outputBuffer = kmalloc(MBR_SECTOR_SIZE);
    memset(outputBuffer, 0, MBR_SECTOR_SIZE);           // Clear the buffer to prevent errors
    if(outputBuffer == NULL){
        // There isn't enough free heap space left! The disk can't be identified.
        return diskInfo;
    }

    ATAWait(basePort, 2000);

    // Set the commands to be sent to either the master or slave disk for this bus
    outb(ATA_DRIVE_HEAD(basePort), masterOrSlave);

    // Send 0 to the low medium and high registers
    outb(ATA_DRIVE_LO(basePort), 0);
    outb(ATA_DRIVE_MID(basePort), 0);
    outb(ATA_DRIVE_HIGH(basePort), 0);

    // Send IDENTIFY command to the disk
    outb(ATA_DRIVE_CMD_STATUS(basePort), ATA_ID_COMMAND);

    uint8 status;

    // Get the current status of the disk. Read multiple times to clear errors and prepare the drive.
    for(int i = 0; i < 4; i++){
        status = inb(ATA_DRIVE_CMD_STATUS(basePort));
    }

    if(status == 0 || status == ATA_FLOATING_BUS){
        // Drive doesn't exist or bus is empty
        kfree(outputBuffer);
        return diskInfo;
    }

    ATAWait(basePort, 2000);
    
    // The drive exists, and will be outputting important information. Save that to the allocated buffer.
    for(int i = 0; i < 256; i++){
        outputBuffer[i] = inw(ATA_DRIVE_DATA(basePort));
        ATAWait(basePort, 2000);
    }

    // Set the current mode and the pointer to the data buffer to the disk info type.
    diskInfo->dataBuffer = outputBuffer;
    diskInfo->supportedModes = GetModes(outputBuffer);
    diskInfo->sizeInBytes = GetDiskSize(diskInfo);
    diskInfo->deviceType = DetectDeviceType(diskInfo);

    // Set the current addressing mode.
    SetMode(diskInfo);

    BIOS_parameter_block_t* header = ReadFat32Header(diskInfo);

    if(strcmp(header->filesystemType, "FAT32") == 0){
        diskInfo->fsType = FS_FAT32;
    }

    // Return the pointer to the buffer
    return diskInfo;
}

// Get the current state of a given drive
void ScanDrive(disk_info_t* disk){
    // Inform the user that the disk is being scanned
    kprintf("Scanning drive %d...\n", disk->diskID);

    if(disk->dataBuffer == NULL){
        // If no hard disk or ROM drive exists at this device ID, the data buffer does not exist.
        kprintf("Drive %d does not exist!\n\n", disk->diskID);
        return;
    }

    // Set a boolean value determining if the device exists or not
    bool driveExists;
    driveExists = false;
    for(int i = 0; i < 256  /* Size of the buffer returned in words */; i++){
        if(disk->dataBuffer != NULL && disk->dataBuffer[i] != 0){
            // Saying the drive got data back
            driveExists = true;
        }
    }
    if(driveExists){
        kprintf("Drive %d exists!\n\n", disk->diskID);
    }else{
        kprintf("Drive %d exists but is empty!\n\n", disk->diskID);
    }
}

// Get the supported modes of the drive. Due to the structs we created, this function is easy.
void ScanSupportedModes(disk_info_t* disk){
    if(disk->dataBuffer == NULL){
        // No disk to detect
        return;
    }

    // Inform the user of the disk the driver will be getting supported modes for
    kprintf("Getting the supported modes for disk %d\n", disk->diskID);

    // If the disk does not exist, there are no supported modes and we can safely return.
    if(disk->supportedModes == NULL){
        return;
    }

    // Check CHS mode and inform the user if it exists (Note that this should always exist)
    if(disk->supportedModes->chs){
        kprintf("CHS mode supported!\n");
    }

    // Check for 28-bit LBA support and inform the user if it exists
    if(disk->supportedModes->lba28){
        kprintf("28-bit LBA supported!\n");
    }

    // Check for 48-bit LBA support and inform the user if it exists
    if(disk->supportedModes->lba48){
        kprintf("48-bit LBA supported!\n");
    }

    // Check for 28-bit PIO mode and inform the user if it exists
    if(disk->supportedModes->pio28){
        kprintf("28-bit PIO supported!\n");
    }

    // Check for 48-bit PIO mode and inform the user if it exists
    if(disk->supportedModes->pio48){
        kprintf("48-bit PIO supported!\n");
    }
}

// Read a given sector of a disk given an LBA offset. Only supporting 48-bit PIO LBA for now.
uint16* ReadSectors(disk_info_t* diskInfo, lba_offset_t lbaOffset, uint16 sectorCount){
    if(diskInfo->CurrentMode != LBA_48_BIT){
        // Unsupported mode
        return;
    }
    uint16 basePort = GetBasePort(diskInfo->diskID);
    uint8 masterSlave = MasterOrSlave(diskInfo->diskID);

    if(masterSlave == ATA_IDENTIFY_MASTER){
        // Select the master disk of the current bus
        outb(ATA_DRIVE_HEAD(basePort), PIO_48_MASTER);
    }else if(masterSlave == ATA_IDENTIFY_SLAVE){
        // Select the slave disk of the current bus
        outb(ATA_DRIVE_HEAD(basePort), LBA_MAGIC | PIO_48_SLAVE);
    }

    outb(ATA_DRIVE_SECTORCOUNT(basePort), ((sectorCount >> 8) & 0xFF));
    outb(ATA_DRIVE_LO(basePort), GET_LBA4(lbaOffset));
    outb(ATA_DRIVE_MID(basePort), GET_LBA5(lbaOffset));
    outb(ATA_DRIVE_HIGH(basePort), GET_LBA6(lbaOffset));

    outb(ATA_DRIVE_SECTORCOUNT(basePort), (sectorCount & 0xFF));
    outb(ATA_DRIVE_LO(basePort), GET_LBA1(lbaOffset));
    outb(ATA_DRIVE_MID(basePort), GET_LBA2(lbaOffset));
    outb(ATA_DRIVE_HIGH(basePort), GET_LBA3(lbaOffset));

    outb(ATA_DRIVE_CMD_STATUS(basePort), READ_SECTORS_EXT);

    // This will truly test my memory management functions. I hope they work like I expected them to.
    uint16* buffer = kmalloc(sectorCount * MBR_SECTOR_SIZE);
    if(buffer == NULL){
        kprintf("There is no buffer!\n");
        return;
    }
    memset(buffer, 0, sectorCount * MBR_SECTOR_SIZE);

    uint8 status;
    for(uint16 i = 0; i < sectorCount; i++){
        // Wait for the drive to be ready
        while ((status = inb(ATA_DRIVE_CMD_STATUS(basePort))) & FLG_BUSY){
            if(status & ERROR){
                kprintf("There was an error reading from the disk!\n");
                //kfree(buffer);
                return;
            }
        }

        // Read the data from the data ports
        for(uint16 j = 0; j < (MBR_SECTOR_SIZE / 2); j++){
            buffer[i * (MBR_SECTOR_SIZE / 2) + j] = inw(ATA_DRIVE_DATA(basePort));
        }
    }

    return buffer;
}


// Write to a given sector of a disk given an LBA offset. Only supporting 48-bit PIO LBA for now.
void WriteSectors(disk_info_t* diskInfo, lba_offset_t lbaOffset, uint16 sectorCount, uint16* buffer){
    if(diskInfo->CurrentMode != LBA_48_BIT){
        // Unsupported mode
        return;
    }
    uint16 basePort = GetBasePort(diskInfo->diskID);
    uint8 masterSlave = MasterOrSlave(diskInfo->diskID);

    if(masterSlave == ATA_IDENTIFY_MASTER){
        // Select the master disk of the current bus
        outb(ATA_DRIVE_HEAD(basePort), PIO_48_MASTER);
    }else if(masterSlave == ATA_IDENTIFY_SLAVE){
        // Select the slave disk of the current bus
        outb(ATA_DRIVE_HEAD(basePort), LBA_MAGIC | PIO_48_SLAVE);
    }

    outb(ATA_DRIVE_SECTORCOUNT(basePort), ((sectorCount >> 8) & 0xFF));
    outb(ATA_DRIVE_LO(basePort), GET_LBA4(lbaOffset));
    outb(ATA_DRIVE_MID(basePort), GET_LBA5(lbaOffset));
    outb(ATA_DRIVE_HIGH(basePort), GET_LBA6(lbaOffset));

    outb(ATA_DRIVE_SECTORCOUNT(basePort), (sectorCount & 0xFF));
    outb(ATA_DRIVE_LO(basePort), GET_LBA1(lbaOffset));
    outb(ATA_DRIVE_MID(basePort), GET_LBA2(lbaOffset));
    outb(ATA_DRIVE_HIGH(basePort), GET_LBA3(lbaOffset));

    outb(ATA_DRIVE_CMD_STATUS(basePort), WRITE_SECTORS_EXT);

    uint8 status;
    for(uint16 i = 0; i < sectorCount; i++){
        // Wait for the drive to be ready
        while ((status = inb(ATA_DRIVE_CMD_STATUS(basePort))) & FLG_BUSY){
            if(status & ERROR){
                kprintf("There was an error reading from the disk!\n");
            }
        }

        // Write the data to the data ports
        for(uint16 j = 0; j < (MBR_SECTOR_SIZE / 2); j++){
            outw(ATA_DRIVE_DATA(basePort), buffer[i * (MBR_SECTOR_SIZE / 2) + j]);
        }
    }

    // Flush the cache because we hate speed (less prone to failure)
    outb(ATA_DRIVE_CMD_STATUS(basePort), ATA_FLUSH_CACHE);
}


void GetCompatibility(disk_info_t* disk){
    if(disk->deviceType == DEVTYPE_ATA){
        kprintf("Device is ATA/PATA compatible!\n");
    }

    if(disk->deviceType == DEVTYPE_ATAPI){
        kprintf("Device is ATAPI/PATAPI!\n");
    }

    if(disk->deviceType == DEVTYPE_SATA){
        kprintf("Device is SATA compatible!\n");
    }

    if(disk->deviceType == DEVTYPE_SATAPI){
        kprintf("Device is SATAPI compatible!\n");
    }
}