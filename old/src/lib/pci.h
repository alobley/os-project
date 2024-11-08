#ifndef PCI_H
#define PCI_H

// The PCI ports to read to and write from to configure devices
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#include "types.h"

// PCI device structure
typedef struct{
    uint8 bus;
    uint8 device;
    uint8 function;
    uint16 vendor_id;
    uint16 device_id;
} pci_device_t;

// Function prototypes
int pci_find_device(uint16 vendor_id, uint16 device_id, pci_device_t* dev);
uint32 pci_read_config_dword(uint8 bus, uint8 device, uint8 function, uint8 offset);

// More functions can be addded later

#endif