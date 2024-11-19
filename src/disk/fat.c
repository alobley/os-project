#include "fat.h"
#include <alloc.h>
#include <string.h>
#include <vga.h>


// Why doesn't this work? It doesn't make sense.
fat_disk_t* ParseFilesystem(disk_t* disk){
    bpb_t* bpb = NULL;
    if(disk->type == PATADISK){
        if(disk->addressing == LBA48){
            bpb = (bpb_t*)ReadSectors(disk, 1, 1);
        }else{
            bpb = (bpb_t*)ReadSectors(disk, 1, 0);
        }
    }else{
        return NULL;
    }

    fat_disk_t* fatDisk = (fat_disk_t*)alloc(sizeof(fat_disk_t));
    fatDisk->parent = disk;
    
    if(bpb->totalSectors == 0){
        fatDisk->volumeSectors = bpb->largeSectorCount;
    }else{
        fatDisk->volumeSectors = bpb->totalSectors;
    }

    if(bpb->sectorsPerFAT == 0){
        fatDisk->fatSize = bpb->ebr.ebr_type.fat32.sectorsPerFat;
    }else{
        fatDisk->fatSize = bpb->sectorsPerFAT;
    }

    if(bpb->rootDirEntries != 0 && bpb->bytesPerSector != 0){
        fatDisk->rootSectors = ((bpb->rootDirEntries * 32) + (bpb->bytesPerSector - 1)) / bpb->bytesPerSector;
    }else{
        fatDisk->rootSectors = 0;
    }

    fatDisk->firstDataSector = bpb->reservedSectors + (bpb->numFATs * bpb->sectorsPerFAT) + fatDisk->rootSectors;

    fatDisk->firstFatSector = bpb->reservedSectors;

    fatDisk->dataSectors = bpb->totalSectors - (bpb->reservedSectors + (bpb->numFATs * bpb->sectorsPerFAT) + fatDisk->rootSectors);

    if(fatDisk->dataSectors != 0 && bpb->sectorsPerCluster != 0){
        fatDisk->totalClusters = fatDisk->dataSectors / bpb->sectorsPerCluster;
    }else{
        fatDisk->totalClusters = 0;
    }

    if(bpb->bytesPerSector == 0){
        //fatDisk->type = FS_EXFAT;
        fatDisk->type = FS_UNSUPPORTED;
    }else if(fatDisk->totalClusters < 4085){
        fatDisk->type = FS_FAT12;
    }else if(fatDisk->totalClusters < 65525){
        fatDisk->type = FS_FAT16;
    }else{
        fatDisk->type = FS_FAT32;
    }

    dealloc(bpb);
    return fatDisk;
}