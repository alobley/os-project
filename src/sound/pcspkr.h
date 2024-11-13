#ifndef PCSPKR_H
#define PCSPKR_H

#include <types.h>
#include <util.h>
#include <io.h>

void PCSP_NoSound();
void PCSP_Beep();
void PCSP_PlaySound(uint16 frequency);
void PCSP_SpeakerInit();
void PCSP_BeepOn();

#endif