#ifndef VIRTIO_H
#define VIRTIO_H

#include "../types.h"

// A special identifier that all virtio devices have
#define VIRTIO_MAGIC_VALUE 0x74726976

// Virtio device features commands
#define VIRTIO_RESET 0x00
#define VERTIO_ACKNOWLEDGE 0x01
#define VERTIO_DRIVER 0x02
#define VIRTIO_DRIVER_OK 0x04
#define VIRTIO_FEATURES_OK 0x08
#define VIRTIO_DEVICE_NEEDS_RESET 0x80

// Supported virtio device versions
#define VIRTIO_LEGACY 0x1
#define VIRTIO_VERSION 0x2

#define VIRTIO_HARD_DRIVE 0x02
#define VIRTIO_GPU 0x16

#define DRIVER_FAILURE -1
#define DRIVER_SUCCESS 0

// The struct that defines virtio registers in an MMIO region. Can set a pointer to this to the physical memory address of the device and use this struct
// To interact with the MMIO.
typedef volatile struct __attribute__((packed)){
    uint32 MagicValue;
    uint32 Version;
    uint32 DeviceID;
    uint32 VendorID;
    uint32 DeviceFeatures;
    uint32 DeviceFeaturesSel;
    uint32 _reserved0[2];
    uint32 DriverFeatures;
    uint32 DriverFeaturesSel;
    uint32 _reserved1[2];
    uint32 QueueSel;
    uint32 QueueNumMax;
    uint32 QueueNum;
    uint32 _reserved2[2];
    uint32 QueueReady;
    uint32 _reserved3[2];
    uint32 QueueNotify;
    uint32 _reserved4[3];
    uint32 InterruptStatus;
    uint32 InterruptACK;
    uint32 _reserved5[2];
    uint32 Status;
    uint32 _reserved6[3];
    uint32 QueueDescLow;
    uint32 QueueDescHigh;
    uint32 _reserved7[2];
    uint32 QueueAvailLow;
    uint32 QueueAvailHigh;
    uint32 _reserved8[2];
    uint32 QueueUsedLow;
    uint32 QueueUsedHigh;
    uint32 _reserved9[21];
    uint32 ConfigGeneration;
    uint32 Config[0];
} virtio_regs;

// This struct describes a virtio device's queue
typedef struct{
    uint64 addr;
    uint32 len;
    uint16 flags;
    uint16 next;
} virtq_desc;

// Available queue describer
typedef struct{
    uint16 flags;
    uint16 idx;
    uint16 ring[];
} virtq_avail;

// Used queue describer
typedef struct{
    uint16 flags;
    uint16 idx;
    struct{
        uint32 id;
        uint32 len;
    } ring[];
} virtq_used;

// Virtio queue
typedef struct{
    virtq_desc* desc;
    virtq_avail* avail;
    virtq_used* used;
    size_t size;
} virtq;

// Throw a critical failure
void PANIC_KERNEL_ERROR();

// Define a queue for a given virtio device
virtq setup_virtqueue(virtio_regs *device);

// Assign the virtio struct to the address of a virtio mdevice memory space
virtio_regs* set_virtio_device(uint64 location);

// Find a virtio device connected through PCI
uint64 find_virtio_device(uint16 vendorID, uint16 deviceID);

// Queue a command to a virtio's device queue
void virtio_enqueue(virtio_regs* dev, virtq* queue, void* item, size_t itemsize);

// Initialize virtio device
uint8 virtio_device_init(virtio_regs* device);

#endif