#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <types.h>
#include <util.h>

// File written by alobley, the author of this project. This is under the MIT license.

typedef struct MemoryMap {
    uint32 size;
    uint64 baseAddr;
    uint64 length;
    uint32 type;        // The type of memory in the region, e.g. available, reserved
} PACKED mboot_mmap_t;

typedef struct {
    uintptr_t modStart;     // Module starting address
    uintptr_t modEnd;       // Module ending address
    char* string;           // String describing the module
    uint32 reserved;
} PACKED module_t;

// The multiboot info structure passed to the kernel. Hardly used now, will be utilized more later.
typedef struct MultibootInfo {
    // System information
    uint32 flags;           // Flags for which fields are valid

    // Memory information (in kb)
    uint32 memLower;
    uint32 memUpper;

    // BIOS number of the boot device
    uint32 bootDevice;

    // Command-line string address
    char* cmdline;

    // Module info
    uint32 modsCount;       // Number of loaded modules
    module_t* modsAddr;     // Address of the first module structure

    // Symbol table and ELF section header information
    union {
        struct {
            uint32 tabsize;
            uint32 strsize;
            uint32 addr;
            uint32 reserved;
        } aout_sym;
        struct {
            uint32 num;
            uint32 size;
            uint32 addr;
            uint32 shndx;
        } elf_sec;
    } syms;

    // Memory map information (this is important)
    uint32 mmapLen;         // Length of the memory map
    mboot_mmap_t* mmapAddr; // Address of the first entry in the memory map

    // Drive information
    uint32 drivesLen;       // Length of the drives structure
    uintptr_t drivesAddr;   // Address of the first drive structure

    // ROM configuration table
    uint32 configTable;

    // Name of the bootloader (pointer to the string)
    char* bootloaderName;

    // Advanced power management table
    uintptr_t apm_table;

    // VESA BIOS Extensions (these could be very helpful)
    uint32 vbeCtrlInfo;
    uint32 vbeModeInfo;
    uint16 vbeMode;
    uint16 vbeInterfaceSeg;
    uint16 vbeInterfaceOff;
    uint16 vbeInterfaceLen;

    // VBE framebuffer information
    uint64 framebufferAddr;
    uint32 framebufferPitch;
    uint32 framebufferWidth;
    uint32 framebufferHeight;
    uint8 framebufferBpp;
    uint8 framebufferType;
    uint8 colorInfo[6];
} PACKED mboot_info_t;


#endif