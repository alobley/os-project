#ifndef ACPI_H
#define ACPI_H

#include "types.h"

#define MAX_CPU_COUNT 4

extern uint32 acpiCpuCount;
extern uint8 acpiCpuIds[MAX_CPU_COUNT];

void AcpiInit();
uint32 AcpiRemapIrq(uint32 irq);

#endif