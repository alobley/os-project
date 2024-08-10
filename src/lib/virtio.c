#include "virtio.h"
#include "pci.h"
#include "memory.h"
#include "vga.h"

// There was a critical error in kernel drivers
void PANIC_KERNEL_ERROR(){
    // Set rax to the driver error code and halt execution
    asm("mov $0xDDDDDDDDDDDDDDDD, %rax");
    kprintf("CRITICAL ERROR - KERNEL STOPPING\n");
    while(true);
}

// Find a virtio device connected via PCI/PCIe
uint64 find_virtio_device(uint16 vendorID, uint16 deviceID){
    pci_device_t dev;
    if(pci_find_device(vendorID, deviceID, &dev) == 0){
        // PCI device found
        uint32 bar0 = pci_read_config_dword(dev.bus, dev.device, dev.function, 0x10);
        return bar0;
    }
    // PCI device not found
    return DRIVER_FAILURE;
}

// Set a pointer to a virtio device registers struct to the MMIO address of the device
virtio_regs* set_virtio_device(uint64 location){
    virtio_regs* virtio_device = (virtio_regs *)location;
    return virtio_device;
}

// Initialize a virtio device
uint8 virtio_device_init(virtio_regs* device){
    if(device->MagicValue != VIRTIO_MAGIC_VALUE){
        // Check if the device is a virtio device. If not, fail.
        return DRIVER_FAILURE;
    }
    if(device->Version != VIRTIO_VERSION && device->Version != VIRTIO_LEGACY){
        // Check if the version of the device is supported (supports virtio version 1 and 2)
        return DRIVER_FAILURE;
    }
    if(device->DeviceID == 0){
        // If the device's device ID is 0, it does not exist, fail.
        return DRIVER_FAILURE;
    }

    // Signal to the device that it was recognized by the driver and it is about to be configured
    device->Status = 0;
    device->Status |= VERTIO_ACKNOWLEDGE;
    device->Status |= VERTIO_DRIVER;

    // Read device features
    uint32 features = device->DeviceFeatures;

    // Write the driver features
    device->DriverFeatures = features;

    // Confirm features
    device->Status |= VIRTIO_FEATURES_OK;
    if(!(device->Status & VIRTIO_FEATURES_OK)){
        return DRIVER_FAILURE;
    }

    // Set up queues
    device->QueueSel = 0;
    uint32 queue_size = device->QueueNumMax;
    device->QueueNum = queue_size;

    // Allocating and initializing queue memory should be done here. Those will need to be added manually.

    device->Status |= VIRTIO_DRIVER_OK;
    
    // Success
    return DRIVER_SUCCESS;
}

// Set up the command queue for the given virtio device
virtq setup_virtqueue(virtio_regs *device){
    // Declare the queue and its size
    virtq queue;
    queue.size = device->QueueNumMax;

    // Allocate memory for the queue
    virtq_desc* desc = (virtq_desc *)kmalloc(sizeof(virtq_desc) * queue.size);
    if(desc == NULL){
        // If there was no memory left to allocate, throw a critical failure
        PANIC_KERNEL_ERROR();
    }

    queue.desc = desc;

    // Allocate memory for the available queue
    virtq_avail* avail = (virtq_avail *)kmalloc(sizeof(virtq_avail) + sizeof(uint16) * queue.size);
    if(avail == NULL){
        // If there was no memory left to allocate, throw a critical failure
        PANIC_KERNEL_ERROR();
    }

    queue.avail = avail;

    // Allocate memory for the used queue
    virtq_used* used = (virtq_used *)kmalloc(sizeof(virtq_used) + sizeof(used->ring[0]) * queue.size);
    if(used == NULL){
        // If there was no memory left to allocate, throw a critical error
        PANIC_KERNEL_ERROR();
    }

    queue.used = used;

    // Set all the data in the queue to 0
    for(size_t i = 0; i < queue.size; i++){
        desc[i].addr = 0;
        desc[i].len = 0;
        desc[i].flags = 0;
        desc[i].next = (i + 1) % queue.size;
    }

    // Set the queue address
    device->QueueDescLow = (uint32)((uint64)desc & UINT_MAX);
    device->QueueDescLow = (uint32)(((uint64)desc >> 32) & UINT_MAX);

    device->QueueAvailLow = (uint32)((uint64)avail & UINT_MAX);
    device->QueueAvailHigh = (uint32)(((uint64)avail >> 32) & UINT_MAX);

    device->QueueUsedLow = (uint32)((uint64)used & UINT_MAX);
    device->QueueUsedHigh = (uint32)(((uint64)used >> 32) & UINT_MAX);

    // Mark the queue as ready
    device->QueueReady = true;

    // Success
    return queue;
}

// Enqueue a command into the virtio device queue
// dev = virtio device
// queue = the virtio device's queue
// item = a pointer to the item to enqueue
// itemsize = the size of the item to enqueue
void virtio_enqueue(virtio_regs* dev, virtq* queue, void* item, size_t itemsize){
    // Get the top of the empty queue space
    uint16 head = queue->avail->idx % queue->size;
    uint16 desc_index = head;

    queue->desc[desc_index].addr = (uint64)item;
    queue->desc[desc_index].len = itemsize;
    queue->desc[desc_index].flags = 0;

    queue->avail->ring[head] = desc_index;

    queue->avail->idx += 1;

    // Notify the device that an instruction has been added to the queue
    dev->QueueNotify = 0;
}
