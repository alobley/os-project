#include "pci.h"
#include "../io.h"
#include "../asm.h"
#include "console.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_VENDOR_ID_OFFSET 0x00
#define PCI_DEVICE_ID_OFFSET 0x02
#define PCI_HEADER_TYPE_OFFSET 0x0E


// Create a PCI address
uint32 CreatePCIAddress(uint8 bus, uint8 device, uint8 function, uint8 offset){
    return (1u << 31) | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC);
}

// Read a PCI device's configuration
uint32 ReadPCIConfig(uint8 bus, uint8 device, uint8 function, uint8 offset){
    uint32 address = CreatePCIAddress(bus, device, function, offset);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

// Write a PCI device's configuration
void WritePCIConfig(uint8 bus, uint8 device, uint8 function, uint8 offset, uint32 value) {
    uint32 address = CreatePCIAddress(bus, device, function, offset);
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

// Find a PCI device by vendor ID and device ID
int FindPCIDevice(uint16 vendorID, uint16 deviceID, pci_device_t* dev){
    // Iterate through each bus
    for (uint16 bus = 0; bus < 256; bus++) {
        // Iterate through each device at each bus
        for (uint8 device = 0; device < 32; device++) {
            // Iterate through every function of each device
            for (uint8 function = 0; function < 8; function++) {
                // Read the vendor ID from the PCI device
                uint16 vid = (uint16)(ReadPCIConfig(bus, device, function, PCI_VENDOR_ID_OFFSET) & 0xFFFF);

                // Is the vendor ID that was recieved the same as the one we're looking for?
                if (vid == vendorID) {
                    // If so, obrain the device ID
                    uint16 did = (uint16)(ReadPCIConfig(bus, device, function, PCI_DEVICE_ID_OFFSET) & 0xFFFF);

                    if (did == deviceID) {
                        // If the device ID was found, assign its attributes to the PCI device struct passed to the function
                        dev->bus = bus;
                        dev->device = device;
                        dev->function = function;
                        dev->vendor_id = vendorID;
                        dev->device_id = deviceID;
                        return 0;  // Found device
                    }
                }
            }
        }
    }
    return -1;  // Device not found
}