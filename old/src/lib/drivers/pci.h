#ifndef PCI_H
#define PCI_H

#include "../types.h"

typedef struct{
    uint8 bus;
    uint8 device;
    uint8 function;
    uint16 vendor_id;
    uint16 device_id;
} pci_device_t;

int FindPCIDevice(uint16 vendorID, uint16 deviceID, pci_device_t* dev);

uint32 ReadPCIConfig(uint8 bus, uint8 device, uint8 function, uint8 offset);

uint32 CreatePCIAddress(uint8 bus, uint8 device, uint8 function, uint8 offset);

void WritePCIConfig(uint8 bus, uint8 device, uint8 function, uint8 offset, uint32 value);

#endif