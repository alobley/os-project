#include "fat.h"
#include <alloc.h>
#include <string.h>
#include <vga.h>


// Get the important filesystem info out of the BPB and return a valid FAT disk if it is in fact valid
fat_disk_t* ParseFilesystem(disk_t* disk){
    if(disk == NULL || (disk->populated == false && disk->removable == true)){
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

    fatDisk->paramBlock = bpb;
    return fatDisk;
}

// "It's simple and easy" they said. WHERE ARE THE FILES??????
// Find a file or directory. LFN unsupported for the time being.
fat_entry_t* SeekFile(fat_disk_t* fatdisk, char* fileName){
    if(fatdisk->fstype == FS_UNSUPPORTED){
        return NULL;
    }

    // Slice and parse the given filename
    char name[11];
    memset(name, ' ', 11);

    int i = 0;

    // Copy the filename part before the dot, up to 8 characters
    while (fileName[i] != '.' && fileName[i] != '\0' && i < 8) {
        // Get the file name part
        name[i] = toupper(fileName[i]);  // Convert to uppercase for 8.3 format compliance
        i++;
    }

    // If a dot is found, handle the extension
    if (fileName[i] == '.') {
        int extIndex = 8;  // Extensions start at the 9th position in the `name` array (index 8)
        i++;  // Move past the dot

        // Copy up to 3 characters of the extension
        for (int j = 0; j < 3 && fileName[i] != '\0'; j++, i++) {
            name[extIndex] = toupper(fileName[i]);  // Convert to uppercase for compliance
            extIndex++;
        }
    }

    // Confirms that the name was generated properly (it is)
    //WriteStrSize(&name[0], 11);
    //WriteStr("\n");

    uint64 rootCluster;
    uint64 rootSector;
    if(fatdisk->fstype == FS_FAT32 || fatdisk->fstype == FS_EXFAT){
        // FAT32 root directory cluster
        rootCluster = fatdisk->paramBlock->ebr.ebr_type.fat32.rootDirCluster;
        rootSector = fatdisk->firstDataSector + (rootCluster - 2) * fatdisk->paramBlock->sectorsPerCluster;
    }else{
        // FAT12/16 root directory handling
        uint64 rootDirSectors = ((fatdisk->paramBlock->rootDirEntries * sizeof(fat_entry_t)) + (fatdisk->paramBlock->bytesPerSector - 1)) / fatdisk->paramBlock->bytesPerSector;
        rootSector = fatdisk->firstFatSector + (fatdisk->paramBlock->numFATs * fatdisk->paramBlock->sectorsPerFAT);
        rootCluster = rootSector; // For FAT12/16, treat the root directory as a "cluster"
    }

    //uint64 rootSector = fatdisk->firstDataSector + (rootCluster - 2) * fatdisk->paramBlock->sectorsPerCluster;

    uint16* buffer = ReadSectors(fatdisk->parent, fatdisk->paramBlock->sectorsPerCluster, rootSector);
    if(buffer == NULL){
        return NULL;
    }


    size_t bytesPerCluster = fatdisk->paramBlock->sectorsPerCluster * fatdisk->paramBlock->bytesPerSector;
    size_t numEntries = bytesPerCluster / (sizeof(fat_entry_t));;

    fat_entry_t* entry = alloc(sizeof(fat_entry_t));
    memset(entry, 0, sizeof(fat_entry_t));

    for(size_t i = 0; i < numEntries; i++){
        fat_entry_t* file = (fat_entry_t* )((uint8*)buffer + i * sizeof(fat_entry_t));
        WriteStrSize(file->name, 11);
        for(int j = 0; j < sizeof(fat_entry_t); j++){
            printk("0x%x ", *((unsigned char*)(file + j)));
        }
        if(file->name[0] == DELETED){
            continue;
        }
        if(file->attributes == 0){
            // End of directory entries
            printk("End of entries!\n");
            break;
        }
        if(file->name[0] != 0){
            if(strncmp(file->name, &name[0], 11)){
                memcpy(entry, file, sizeof(fat_entry_t));
                break;
            }
        }
    }

    if(entry->fileSize == 0){
        dealloc(buffer);
        dealloc(entry);
        return NULL;
    }

    dealloc(buffer);
    return entry;
}