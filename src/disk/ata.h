#ifndef ATA_H
#define ATA_H

#include <types.h>
#include <io.h>
#include <util.h>

#define PRIMARY_ATA_IRQ 14
#define SECONDAY_ATA_IRQ 15

#define MAX_DRIVES 8

// Addressing support types
#define CHS_ONLY 0
#define LBA28 1
#define LBA48 2

#define PATADISK 0      // Hard drive, readable and writable
#define PATAPIDISK 1    // ROM drive
#define SATADISK 2      // Disk is attached via SATA.

typedef struct ataDisk {
    uint8 driveNum;     // Drive number (determined by me)
    uint16 base;        // Base port
    uint16 ctrl;        // Control register
    uint64 size;        // Total sectors
    uint8 type;         // PATA, PATAPI, SATA, SATAPI
    uint8 addressing;   // CHS, LBA28, or LBA48
    bool slave;         // Whether the device is a slave or not
    uint16* infoSector; // The 256 16-bit values read when the disk was identified
} disk_t;

disk_t* IdentifyDisk(uint8 diskNum);

#endif