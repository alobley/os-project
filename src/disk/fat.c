#include "fat.h"
#include <alloc.h>

fat_disk_t* ParseFilesystem(disk_t* disk, bpb_t* bpb){
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

    if(bpb->rootDirEntries != 0){
        fatDisk->rootSectors = ((bpb->rootDirEntries * 32) + (bpb->bytesPerSector - 1)) / bpb->bytesPerSector;
    }else{
        fatDisk->rootSectors = 0;
    }

    fatDisk->firstDataSector = bpb->reservedSectors + (bpb->numFATs * bpb->sectorsPerFAT) + fatDisk->rootSectors;

    fatDisk->firstFatSector = bpb->reservedSectors;

    fatDisk->dataSectors = bpb->totalSectors - (bpb->reservedSectors + (bpb->numFATs * bpb->sectorsPerFAT) + fatDisk->rootSectors);

    fatDisk->totalClusters = fatDisk->dataSectors / bpb->sectorsPerCluster;

    if(bpb->bytesPerSector == 0){
        fatDisk->type = FS_EXFAT;
    }else if(fatDisk->totalClusters < 4085){
        fatDisk->type = FS_FAT12;
    }else if(fatDisk->totalClusters < 65525){
        fatDisk->type = FS_FAT16;
    }else{
        fatDisk->type = FS_FAT32;
    }
}