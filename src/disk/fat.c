#include "fat.h"
#include <alloc.h>
#include <string.h>
#include <vga.h>


// Why doesn't this work? It doesn't make sense.
fat_disk_t* ParseFilesystem(disk_t* disk){
    if(disk == NULL){
        return NULL;
    }
    
    bpb_t* bpb = NULL;
    bpb = (bpb_t*)ReadSectors(disk, 1, 0);

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

    if(strncmp(bpb->ebr.ebr_type.fat32.systemID, "FAT32", 5)){
        fatDisk->fstype = FS_FAT32;
    }else if(strncmp(bpb->ebr.ebr_type.fat1216.fsType, "FAT12", 5)){
        fatDisk->fstype = FS_FAT12;
    }else if(strncmp(bpb->ebr.ebr_type.fat1216.fsType, "FAT16", 5)){
        fatDisk->fstype = FS_FAT16;
    }else{
        fatDisk->fstype = FS_UNSUPPORTED;
    }

    dealloc(bpb);
    return fatDisk;
}