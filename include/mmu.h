#ifndef MMU_H
#define MMU_H

#include "common.h"

typedef struct {
	BYTE in_bios;
	BYTE* bios;
	BYTE* rom;
	BYTE* eram;
	BYTE* wram;
	BYTE* zram;
#ifdef __DEBUG__
	BYTE* ram;
#endif
} MMU;

void MMU_init();
void MMU_reset();

void MMU_load_bios(BYTE*);

BYTE MMU_read_8(WORD);
void MMU_write_8(WORD, BYTE);
WORD MMU_read_16(WORD);
void MMU_write_16(WORD, WORD);

#endif
