#ifndef GPU_H
#define GPU_H

#include "common.h"

#define MODE_OAM 2
#define MODE_VRAM 3
#define MODE_HBLANK 0
#define MODE_VBLANK 1

typedef struct {
	BYTE vram[0x2000];
	BYTE tileset[384 * 8 * 8];
	BYTE screen[160 * 144 * 4];
	BYTE mode;
	DWORD mode_clock;
	BYTE line;

	BYTE scroll_y;     // rw
	BYTE scroll_x;     // rw
	BYTE cur_scanline; // r

	RGB bg_palette[4];

	// LCD and GPU control registers (rw)
	BYTE bg_on;           // bit 0
	BYTE sprites_on;      // bit 1
	BYTE sprites_size;    // bit 2 (0 = 8x8, 1 = 16x16)
	BYTE bg_tile_map;     // bit 3
	BYTE bg_tile_set;     // bit 4
	BYTE window_on;       // bit 5
	BYTE window_tile_map; // bit 6
	BYTE display_on;      // bit 7
} GPU;

void GPU_init();
void GPU_reset();
void GPU_set_bg_palette(RGB*);
void GPU_update_tile(WORD);
void GPU_step();

#endif
