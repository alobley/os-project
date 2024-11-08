#include "fat32.h"
#include "ata.h"
#include "../memory.h"
#include "../string.h"
#include "console.h"
#include "../math.h"

// Definitions:

// Bad/Defective sector: a sector whose contents cannot be read or written

// File: a named stream of bytes representing a collection of information

// Sector: a unit of data that can be accessed independently of other units on the media

// Cluster: a unit of allocation containing a set of logically contiguous sectors
//      -Referred to by a cluster number "N"
//      - Allocation of a file must be an integral multiple of a cluster

// Partition: an extent of sectors in a volume

// Volume: logically contiguous sector address space as specifies in the relevant standard for recording

// Arithmetic notation:
//      - ip(x) means the integer part of x
//      - ciel(x) means the minimum integer that is greater than x (x + 1 most likely)
//      - rem(x, y) means the remainder of the integer division of x by y

// FAT regions:
//  0: reserved
//  1: FAT region
//  2: Root directory region (IMPORTANT: does not exist on FAT32 volumes)
//  3: File and directory data region

// All data structures on the disk are little-endian (thank god)

// The BPB (BIOS parameter block) is the first sector of the volume


// To read the file system, we need to find out how big a sector and cluster are.


BIOS_parameter_block_t* ReadFat32Header(disk_info_t* disk){
    if(disk == NULL || disk->CurrentMode != LBA_48_BIT){
        return NULL;
    }

    uint16 sectorCount = 1;
    lba_offset_t lbaOffset = FAT32_BOOT_SECTOR_OFFSET;

    uint16* buffer = ReadSectors(disk, lbaOffset, sectorCount);
    if(buffer == NULL){
        return NULL;
    }

    BIOS_parameter_block_t* header = (BIOS_parameter_block_t* )buffer;

    return header;
}

fat_fs_t* DefineFileSystem(BIOS_parameter_block_t* fsHeader, disk_info_t* disk){
    fat_fs_t* fs = kmalloc(sizeof(fat_fs_t));
    if(fs == NULL){
        return NULL;
    }

    fs->totalSectors = (fsHeader->totalSectors16 == 0) ? fsHeader->totalSectors32 : fsHeader->totalSectors16;
    fs->fatSize = (fsHeader->fatSize16 == 0) ? fsHeader->fatSize32 : fsHeader->fatSize16;
    fs->rootDirSectors = ((fsHeader->rootEntryCount * 32) + (fsHeader->bytesPerSector - 1)) / fsHeader->bytesPerSector;
    fs->firstDataSector = fsHeader->reservedSectorCount + (fsHeader->numFATs * fs->fatSize) + fs->rootDirSectors;
    fs->firstFatSector = fsHeader->reservedSectorCount;
    fs->totalDataSectors = fs->totalSectors - (fsHeader->reservedSectorCount + (fsHeader->numFATs * fs->fatSize) + fs->rootDirSectors);
    fs->totalClusters = fs->totalDataSectors / fsHeader->sectorsPerCluster;

    if(disk->fsType == FS_FAT32){
        fs->fsType = FS_FAT32;
    }

    fs->rootCluster = fsHeader->rootCluster;
    fs->sectorsPerCluster = fsHeader->sectorsPerCluster;

    return fs;
}

uint32 GetFirstSector(uint32 cluster, fat_fs_t* fs){
    return ((cluster - 2) * fs->sectorsPerCluster) + fs->firstDataSector;
}

int ReadLongFileName(lfn_entry_t* entry, char* name){
    int length = 0;
    int idx = 0;

    int order = entry->attribute;

    while(order != 0x0F){
        lfn_entry_t* lfnEntry = (lfn_entry_t* )entry;
        if(lfnEntry->order != (order & 0x0F)){
            break;
        }

        for(int i = 0; i < 5; i++){
            name[idx++] = lfnEntry->firstChars[i] & 0xFF;
            name[idx++] = (lfnEntry->firstChars[i] >> 8) & 0xFF;
        }

        for (int i = 0; i < 6; i++) {
            name[idx++] = lfnEntry->secondChars[i] & 0xFF;
            name[idx++] = (lfnEntry->secondChars[i] >> 8) & 0xFF;
        }
        
        for (int i = 0; i < 2; i++) {
            name[idx++] = lfnEntry->finalChars[i] & 0xFF;
            name[idx++] = (lfnEntry->finalChars[i] >> 8) & 0xFF;
        }

        entry++;
        order++;
    }

    return length;
}

#define NO_ISSUE 0
#define DELETED 0xE5

#pragma GCC push_options
#pragma GCC optimize("O0")
char* ParseName(char* name, char* extension){
    char* trueName = (char* )kmalloc(256);
    memset(trueName, 0, 256);
    int i = 0;
    while(i < 256){
        trueName[i] = name[i];
        i++;
        if(name[i] == ' ' || name[i] == '\0'){
            break;
        }
    }

    trueName[i] = '.';
    i++;

    for(int j = 0; j < 3; j++){
        trueName[i] = extension[j];
        i++;
    }

    kfree(trueName + i + 5);

    return trueName;
}
#pragma GCC pop_options

void ParseRoot(fat_fs_t* fs, disk_info_t* disk){
    uint32 rootStart = fs->firstDataSector + ((fs->rootCluster - 2) * fs->sectorsPerCluster);
    
    uint16* buffer = ReadSectors(disk, rootStart, fs->sectorsPerCluster);

    size_t numEntries = FAT32_SECTOR_SIZE / sizeof(fat_cluster_t);
    
    for(size_t i = 0; i < numEntries; ++i){
        fat_cluster_t* entry = (fat_cluster_t* )(buffer + i * sizeof(fat_cluster_t));

        if(entry->filename[0] == (uint8)NO_ISSUE){
            // We found them all
            break;
        }

        if(entry->filename[0] == (uint8)DELETED){
            // Entry was deleted
            continue;
        }

        if(entry->attributes == ATTRIBUTE_LFN){
            char fullName[256];
            int nameLen = ReadLongFileName((lfn_entry_t* )entry, fullName);

            char* trueName = ParseName(&fullName[0], entry->extension);

            fullName[nameLen] = '\0';

            kprintf("File name: ");
            kprintf(trueName);
            kfree(trueName);
            kprintf("\n");
        }else{
            char* trueName = ParseName(entry->filename, entry->extension);
            kprintf("\nFile name: ");
            kprintf(trueName);
            kfree(trueName);
            kprintf("\n");
        }

        kprintf("Attributes: 0x%x\n", entry->attributes);
        kprintf("File size: %llu bytes\n\n", entry->fileSize);

        // Will parse and make a data structure later
    }
}

void* LoadFile(fat_fs_t* fs, disk_info_t* disk, void* loadAddress, char* fileName){
    uint32 rootStart = fs->firstDataSector + ((fs->rootCluster - 2) * fs->sectorsPerCluster);
    
    uint16* buffer = ReadSectors(disk, rootStart, fs->sectorsPerCluster);

    size_t numEntries = FAT32_SECTOR_SIZE / sizeof(fat_cluster_t);

    void* fileData = NULL;
    
    for(size_t i = 0; i < numEntries; i++){
        fat_cluster_t* entry = (fat_cluster_t* )(buffer + i * sizeof(fat_cluster_t));

        if(entry->filename[0] == NO_ISSUE){
            // File not found
            return NULL;
        }

        if(entry->filename[0] == (uint8 )DELETED){
            // Entry was deleted
            return NULL;
        }

        if(entry->attributes == ATTRIBUTE_LFN){
            char fullName[256];
            int nameLen = ReadLongFileName((lfn_entry_t* )entry, fullName);

            char* trueName = ParseName(&fullName[0], entry->extension);

            fullName[nameLen] = '\0';

            if(strcmp(trueName, fileName) == 0){
                kfree(trueName);
                break;
            }
        }else{
            char* trueName = ParseName(entry->filename, entry->extension);
            if(strcmp(trueName, fileName) == 0){
                kfree(trueName);
                break;
            }
        }

        if(loadAddress == NULL){
            fileData = kmalloc(entry->fileSize);
        }else{
            fileData = loadAddress;
        }

        uint32 bytesRemaining = entry->fileSize;
        uint32 cluster = (entry->highAddr << 16) | (entry->lowAddr);

        kprintf("Value of i: %llu\n", i);
        kprintf("Number of entries: %llu\n", numEntries);

        while(cluster >= 2 && cluster < 0xFFFFFF8 && bytesRemaining > 0){
            uint32 sectorCount = min(fs->sectorsPerCluster, (bytesRemaining + FAT32_SECTOR_SIZE - 1) / FAT32_SECTOR_SIZE);
            buffer = ReadSectors(disk, cluster, sectorCount);

            buffer += sectorCount * FAT32_SECTOR_SIZE;
            bytesRemaining -= sectorCount * FAT32_SECTOR_SIZE;

            uint32 fatSector = fs->firstFatSector + (cluster * 4) / FAT32_SECTOR_SIZE;
            uint32 fatOffset = (cluster * 4) % FAT32_SECTOR_SIZE;

            uint16* fatBuffer = ReadSectors(disk, fatSector, 1);

            cluster = *((uint32*)((uint8*)&fatBuffer + fatOffset)) & 0x0FFFFFFF;
        }
    }

    kfree(buffer);
    return fileData;
}