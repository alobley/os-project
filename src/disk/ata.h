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

#define CD_SECTORSIZE 4048          // CDROM and DVDROM sector size
#define HD_SECTORSIZE 512           // The size of one sector on a typical hard disk

typedef struct ataDisk {
    uint8 driveNum;     // Drive number (determined by me)
    uint16 base;        // Base port
    uint16 ctrl;        // Control register
    uint64 size;        // Total size of the disk in sectors
    uint16 sectorSize;  // The size of one sector
    uint8 type;         // PATA, PATAPI, SATA, SATAPI
    uint8 addressing;   // CHS, LBA28, or LBA48
    bool slave : 1;     // Whether the device is a slave or not
    bool packet : 1;    // Whether the disk is a packet disk or not
    bool removable : 1; // Whether or not the device is a removable disk
    bool populated : 1; // The device is populated with a disk (removable media only)
    bool reserved : 4;  // Spare flags in this byte
    uint16* infoBuffer; // The 256 16-bit values read when the disk was identified
} PACKED disk_t;

disk_t* IdentifyDisk(uint8 diskNum);

#endif