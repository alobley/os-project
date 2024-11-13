#include "pci.h"
#include "io.h"

// Create a PCI address
static inline uint32 pci_create_address(uint8 bus, uint8 device, uint8 function, uint8 offset){
    return (1u << 31) | (bus << 16) | (device << 11) | (function << 8) | (offset & 0xFC);
}

// Read a PCI device's configuration
uint32 pci_read_config_dword(uint8 bus, uint8 device, uint8 function, uint8 offset){
    uint32 address = pci_create_address(bus, device, function, offset);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

// Find a PCI device by vendor ID and device ID
int pci_find_device(uint16 vendorID, uint16 deviceID, pci_device_t* dev){
    uint8 bus, device, function;
    // Iterate through each bus
    for (bus = 0; bus < 256; bus++) {
        // Iterate through each device at each bus
        for (device = 0; device < 32; device++) {
            // Iterate through every function of each device
            for (function = 0; function < 8; function++) {
                // Read the vendor ID from the PCI device
                uint16 vid = pci_read_config_dword(bus, device, function, 0);

                // Is the vendor ID that was recieved the same as the one we're looking for?
                if (vid == vendorID) {
                    // If so, obrain the device ID
                    uint16 did = pci_read_config_dword(bus, device, function, 2);

                    // The program simply does not do this part. What in the actual heck??? It doesn't return and the code underneath this does not get executed.
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