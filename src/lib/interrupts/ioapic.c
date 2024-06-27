#include "ioapic.h"
#include "../vga.h"
#include "../io.h"


#define IOREGSEL 0x00
#define IOWIN 0x10

#define IOAPIXID 0x00
#define IOAPICVER 0x01
#define IOAPICARB 0x02
#define IOREDTBL 0x10

uint8 *ioApicAddr = (uint8 *)0xFEC00000;

static void IoApicOut(uint8* base, uint8 reg, uint32 val){
    mmiowritel(base + IOREGSEL, reg);
    mmiowritel(base + IOWIN, val);
}

static uint32 IoApicIn(uint8* base, uint8 reg){
    mmiowritel(base + IOREGSEL, reg);
    return mmioreadl(base + IOWIN);
}

void IoApicSetEntry(uint8* base, uint8 index, uint64 data){
    IoApicOut(base, IOREDTBL + index * 2, (uint32)data);
    IoApicOut(base, IOREDTBL + index * 2 + 1, (uint32)(data >> 32));
}

void IoApicInit(){
    uint32 x = IoApicIn(ioApicAddr, IOAPICVER);
    uint32 count = ((x >> 16) & 0xFF) + 1;

    for(uint32 i = 0; i < count; ++i){
        IoApicSetEntry(ioApicAddr, i, 1 << 16);
    }
}