#ifndef FAT32_H
#define FAT32_H

#include "../types.h"
#include "ata.h"

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
    uint8 signiature;
    uint32 volumeID;
    char volumeLabel[11];
    char filesystemType[8];
    uint8 bootCode[420];        // 420 bytes of boot code
    uint16 bootSig;             // The boot signiature of the disk (is 0xAA55)
} __attribute__((packed)) BIOS_parameter_block_t;

#define FAT32_BOOT_SECTOR_OFFSET 0
#define FAT32_SECTOR_SIZE 512

BIOS_parameter_block_t* ReadFat32Header(disk_info_t* disk);

#define NO_ATTRIBUTE 0x00
#define ATTRIBUTE_READONLY 0x01
#define ATTRIBUTE_HIDDEN 0x02
#define ATTRIBUTE_SYSTEM 0x04
#define ATTRIBUTE_VOLUME_ID 0x08
#define ATTRIBUTE_DIRECTORY 0x10
#define ATTRIBUTE_ARCHIVE 0x20

// This means it's a long file name entry
#define ATTRIBUTE_LFN (ATTRIBUTE_READONLY | ATTRIBUTE_HIDDEN | ATTRIBUTE_SYSTEM | ATTRIBUTE_VOLUME_ID)

// Take a date from a file entry and 3 writable variables 1 byte or larger and parse them correctly.
#define PARSE_DATE(date, day, month, year) day = (date & 0b11111); month = (date >> 5) & 0b1111; year = (date >> 9) & 0b1111111


// Standard FAT 8.3 cluster entry
typedef struct cluster {
    char filename[8];                   // Cluster name. If first byte = 0xE5, it has been deleted.
    char extension[3];                  // The file extension
    uint8 attributes;                   // Attributes of the cluster. LFN entry exists if the LFN attribute is shown.
    uint8 reserved;                     // Reserved for use by the NT kernel. I wonder what Windows does with this.
    uint8 creationTime;                 // The time it took to create in hundredths of a second. Not particularly useful.
    uint16 timeOfCreation;              // The time of day the cluster was created (Bits 11-15 hour, bits 5-10 minutes, bits 0-4 seconds). Seconds must be multiplied by 2.
    uint16 creationDate;                // Date the cluster was created on (bits 9-15 year, bits 5-8 month, bits 0-4 day).
    uint16 lastAccessed;                // The last date this cluster was accessed. Same format as creation date.
    uint16 highAddr;                    // The high 16 bits of the entry's first cluster number.
    uint16 lastModified;                // The last time this cluster was modified. Same format as creation time.
    uint16 lastModifiedDate;            // The date of the last modification of this entry. Same format as creation date.
    uint16 lowAddr;                     // The low 16 bits of the entry's first cluster number. This along with the high address are used to find the first cluster for this entry.
    uint32 fileSize;                    // Size of the file in bytes. Helpful for many reasons.
} __attribute__((packed)) fat_cluster_t;


// A long file name entry. Typically the actual chars are in the upper byte, and it is always null terminated.
typedef struct lfn_entry {
    uint8 order;            // Order of this entry in a sequence of long file name entries. Lets you know where in the file's name the characters should be placed.
    int16 firstChars[5];    // The first five two-byte characters (oh no) of this entry
    uint8 attribute;        // Always 0x0F (LFN attribute)
    uint8 entryType;        // The type of long entry. Zero for name entries.
    uint8 checksum;         // Checksum created of the short file name when the file was created. The short file name can change on a platform that doesn't support LFNs.
    int16 secondChars[6];   // The next 6 two-byte chars of this entry
    uint16 zero;            // Always 0
    int16 finalChars[2];    // The final two two-byte chars of this entry
    uint16 padding;         // The padding at the end of the long file name. Always 0xFFFF. May not be needed.
} __attribute__((packed)) lfn_entry_t;

// Set a typedef for a cluster that has a long file name
typedef struct lfn_cluster {
    lfn_entry_t* lfnEntry;
    fat_cluster_t* fatEntry;
} long_name_cluster_t;

typedef struct fat_fs {
    uint32 totalSectors;       // Total sectors in the volume
    uint32 fatSize;            // FAT size in sectors
    uint32 rootDirSectors;     // Is zero in FAT32
    uint32 firstDataSector;    // The first sector on the disk where files and directories can be stored
    uint32 firstFatSector;     // The first sector in the FAT
    uint32 totalDataSectors;   // The total amount of data sectors
    uint32 totalClusters;      // The total amount of clusters on the disk
    uint8 fsType;              // The driver only supports FAT32, so it must be FAT32
    uint32 rootCluster;
    uint8 sectorsPerCluster;
} fat_fs_t;

fat_fs_t* DefineFileSystem(BIOS_parameter_block_t* fsHeader, disk_info_t* disk);
void ParseRoot(fat_fs_t* fs, disk_info_t* disk);
void* LoadFile(fat_fs_t* fs, disk_info_t* disk, void* loadAddress, char* fileName);

#endif