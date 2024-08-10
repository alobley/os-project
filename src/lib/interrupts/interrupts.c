#include "interrupts.h"
#include "idt.h"
#include "pic.h"
#include "../asm.h"
#include "../time.h"

extern void pit_interrupt();
extern void spurious_interrupt();


void IntrInit(){
    cli();

    IdtInit();
    PicInit();
    InitializePIT();
    IdtSetHandler(INT_TIMER, INTERRUPT_GATE, &pit_interrupt);
    IdtSetHandler(INT_SPURIOUS, INTERRUPT_GATE, &spurious_interrupt);

    //LocalApicInit();
    //IoApicInit();

    sti();
}