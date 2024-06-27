#include "pic.h"
#include "interrupts.h"
#include "../io.h"

#define PIC1_CMD 0x0020
#define PIC1_DATA 0x0021
#define PIC2_CMD 0x00A0
#define PIC2_DATA 0x00A1

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_ADI 0x04
#define ICW1_LTIM 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x04
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10

void PicInit(){
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

    outb(PIC1_DATA, IRQ_BASE);
    outb(PIC2_DATA, IRQ_BASE + 8);

    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}