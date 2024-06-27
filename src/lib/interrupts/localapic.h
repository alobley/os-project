#ifndef LOCALAPIC_H
#define LOCALAPIC_H

#include "../types.h"

extern uint8 *localApicAddr;

void LocalApicInit();

uint32 LocalApicGetId();
void LocalApicSendInit(uint32 apic_id);
void LocalApicSendStartup(uint32 apic_id, uint32 vector);

#endif