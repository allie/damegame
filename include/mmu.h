#ifndef MMU_H
#define MMU_H

#include "common.h"

typedef struct {
	BYTE bios[256];
	BYTE in_bios;
	BYTE* eram;
	BYTE* wram;
	BYTE* zram;
#ifdef __DEBUG__
	BYTE* ram;
#endif
} MMU;

void MMU_init();
void MMU_reset();

BYTE MMU_read_8(WORD addr);
void MMU_write_8(WORD addr, BYTE val);
WORD MMU_read_16(WORD addr);
void MMU_write_16(WORD addr, WORD val);

#endif
