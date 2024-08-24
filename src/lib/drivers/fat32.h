#ifndef FAT32_H
#define FAT32_H

#include "../types.h"

typedef struct {
    uint8 jmp[3];
    char oemName[8];
    uint16 bytesPerSector;
    uint8 sectorsPerCluster;
    uint16 reservedSectorCount;
    uint8 numFATs;
    uint16 rootEntryCount;
    uint16 totalSectors16;
    uint8 mediaDescriptor;
    uint16 fatSize16;
    uint16 sectorsPerTrack;
    uint16 numberOfHeads;
    uint32 hiddenSectors;
    uint32 totalSectors32;
    uint32 fatSize32;
    uint16 flags;
    uint16 version;
    uint32 rootCluster;
    uint16 fsInfo;
    uint16 backupBootSector;
    uint8 reserved[12];
    uint8 driveNum;
    uint8 reserved1;
    uint8 bootSig;
    uint32 volumeID;
    uint8 volumeLabel[11];
    uint8 filesystemType[8];
} __attribute__((packed)) BIOS_parameter_block_t;

#define FAT32_BOOT_SECTOR_OFFSET 0
#define FAT32_SECTOR_SIZE 512

#endif