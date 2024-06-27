#include "acpi.h"
#include "../vga.h"
#include "../interrupts/ioapic.h"
#include "../interrupts/localapic.h"

// NOTE: This is for multiple CPUs. I will be fine without it for now.

uint32 acpiCpuCount;
uint8 acpiCpuIds[MAX_CPU_COUNT];

typedef struct AcpiHeader {
    uint32 signiature;
    uint32 length;
    uint8 revision;
    uint8 checksum;
    uint8 oem[6];
    uint8 oemTableId[8];
    uint32 oemRevision;
    uint32 creatorId;
    uint32 creatorRevision;
} __attribute__((packed)) AcpiHeader;

typedef struct AcpiFadt{
    AcpiHeader header;
    uint32 firmwareControl;
    uint32 dsdt;
    uint8 reserved;
    uint8 preferredPMProfile;
    uint16 sciInterrupt;
    uint32 smiCommandPort;
    uint8 acpiEnable;
    uint8 acpiDisable;
} __attribute__((packed)) AcpiFadt;

typedef struct AcpiMadt{
    AcpiHeader header;
    uint32 localApicAddr;
    uint32 flags;
} __attribute__((packed)) AcpiMadt;

typedef struct ApicHeader {
    uint8 type;
    uint8 length;
} __attribute__((packed)) ApicHeader;

#define APIC_TYPE_LOCAL_APIC 0
#define APIC_TYPE_IO_APIC 1
#define APIC_TYPE_INTERRUPT_OVERRIDE 2

typedef struct ApicLocalApic{
    ApicHeader header;
    uint8 acpiProcessorId;
    uint8 apicId;
    uint32 flags;
} __attribute__((packed)) ApicLocalApic;

typedef struct ApicIoApic{
    ApicHeader header;
    uint8 ioApicId;
    uint8 reserved;
    uint32 ioApicAddress;
    uint32 globalSystemInterruptBase;
} __attribute__((packed)) ApicIoApic;

typedef struct ApicInterruptOverride {
    ApicHeader header;
    uint8 bus;
    uint8 source;
    uint32 interrupt;
    uint16 flags;
}__attribute__((packed)) ApicInterruptOverride;

static AcpiMadt *s_madt;

static void AcpiParseFacp(AcpiFadt *facp){
    if(facp->smiCommandPort){
        WriteStr("Enabling ACPI", 0, 0);
    }else{
        WriteStr("ACPI Already Enabled!", 0, 0);
    }
}

static void AcpiParseApic(AcpiMadt* madt){
    s_madt = madt;

    localApicAddr = (uint8* )(uintptr_t)madt->localApicAddr;

    uint8* p = (uint8* )(uintptr_t)madt->localApicAddr;
    uint8* end = (uint8* )madt + madt->header.length;

    while(p < end){
        ApicHeader *header = (ApicHeader *)p;
        uint8 type = header->type;
        uint8 length = header->length;

        if(type == APIC_TYPE_LOCAL_APIC){
            ApicLocalApic *s = (ApicLocalApic* )p;
        }
    }
}