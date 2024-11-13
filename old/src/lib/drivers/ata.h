#ifndef IDE_H
#define IDE_H

#include "../types.h"

#define MBR_SECTOR_SIZE 512

#define MAX_SUPPORTED_DRIVES 7          // Including drive 0! This is 8 drives.

#define PRIMARY_BUS_IRQ 14
#define SECONDARY_BUS_IRQ 15

#define TOTAL_MODES 4

#define DEVTYPE_NONE 0xFFFFFFFF

#define NO_MODE 0
#define CHS_MODE 1
#define PIO_28_BIT 2
#define PIO_48_BIT 3
#define LBA_28_BIT 4
#define LBA_48_BIT 5

#define DEVTYPE_ATA 0
#define DEVTYPE_ATAPI 1
#define DEVTYPE_SATA 2
#define DEVTYPE_SATAPI 3

#define FS_NONE_UNSUPPORTED 0
#define FS_FAT32 1
#define FS_ISO 2

typedef unsigned long long lba_offset_t;

typedef struct ModeSupport{
    bool pio28;
    bool pio48;
    bool lba28;
    bool lba48;
    bool chs;
} mode_support_t;

typedef struct DiskInfo {
    uint8 diskID;
    uint16* dataBuffer;
    mode_support_t* supportedModes;
    uint8 CurrentMode;
    uint32 deviceType;
    uint64 sizeInBytes;
    uint16 basePort;
    uint8 fsType;
} disk_info_t;

disk_info_t* InitializeDisk(uint8 deviceID);
void ScanDrive(disk_info_t* disk);
void ScanSupportedModes(disk_info_t* disk);
uint16* ReadSectors(disk_info_t* diskInfo, lba_offset_t lbaOffset, uint16 sectorCount);
void WriteSectors(disk_info_t* diskInfo, lba_offset_t lbaOffset, uint16 sectorCount, uint16* buffer);
void GetCompatibility(disk_info_t* disk);

#endif