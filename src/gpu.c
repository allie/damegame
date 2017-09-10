#include <string.h>
#include "gpu.h"
#include "cpu.h"
#include "lcd.h"
#include <sdl2/sdl.h>

GPU gpu;
extern CPU cpu;

static void GPU_render_scanline() {
	WORD map_addr = gpu.bg_tile_map ? 0x1C00 : 0x1800;
	map_addr += ((gpu.line + gpu.scroll_y) & 0xFF) >> 3;

	WORD line_off = gpu.scroll_x >> 3;
	WORD y = (gpu.line + gpu.scroll_y) & 0x7;
	WORD x = gpu.scroll_x & 0x7;

	int screen_off = gpu.line * 160 * 4;
	int tile = gpu.vram[map_addr + line_off];

	if (gpu.bg_tile_set == 1 && tile < 128) {
		tile += 256;
	}

	for (int i = 0; i < 160; i++) {
		RGB color = gpu.bg_palette[gpu.tileset[(tile * 64) + (y * 8) + x]];
		gpu.screen[screen_off] = color.r;
		gpu.screen[screen_off + 1] = color.g;
		gpu.screen[screen_off + 2] = color.b;
		screen_off += 3;

		if (++x == 8) {
			x = 0;
			line_off = (line_off + 1) & 31;
			tile = gpu.vram[map_addr + line_off];
			if (gpu.bg_tile_set == 1 && tile < 128) {
				tile += 256;
			}
		}
	}
}

void GPU_init() {
	RGB greyscale[4] = {
		{255, 255, 255},
	    {192, 192, 192},
	    {96, 96, 96},
	    {0, 0, 0}
	};

	GPU_set_bg_palette(greyscale);
	GPU_reset();
}

void GPU_set_bg_palette(RGB* palette) {
	memcpy(gpu.bg_palette, palette, 4 * sizeof(RGB));
}

void GPU_reset() {
	memset(gpu.vram, 0, 0x2000);
	memset(gpu.screen, 0, 160 * 144 * 3);
	gpu.mode = MODE_HBLANK;
	gpu.mode_clock = 0;
	gpu.line = 0;
}

void GPU_update_tile(WORD addr) {
	addr &= 0x1FFE;

	int tile = (addr >> 4) & 0x1FF;
	int y = (addr >> 1) & 0x7;

	int sx;
	for (int x = 0; x < 8; x++) {
		sx = 1 << (7 - x);
		int val = ((gpu.vram[addr] & sx) ? 1 : 0) + ((gpu.vram[addr + 1] & sx) ? 2 : 0);
		gpu.tileset[(tile * 64) + (y * 8) + x] = val;
	}
}

// Period                           GPU mode     Cycles
// -------------------------------------------------------------
// Scanline (accessing OAM)         2            80
// Scanline (accessing VRAM)        3            172
// Horizontal blank                 0            204
// One line (scan and blank)		             456
// Vertical blank                   1            4560 (10 lines)
// Full frame (scans and vblank)                 70224

void GPU_step() {
	gpu.mode_clock += cpu.ins_clock.t;

	switch (gpu.mode) {
	case MODE_OAM:
		if (gpu.mode_clock >= 80) {
			gpu.mode_clock = 0;
			gpu.mode = MODE_VRAM;
		}
		break;

	case MODE_VRAM:
		if (gpu.mode_clock >= 172) {
			gpu.mode_clock = 0;
			gpu.mode = MODE_HBLANK;
			GPU_render_scanline();
		}
		break;

	case MODE_HBLANK:
		if (gpu.mode_clock >= 204) {
			gpu.mode_clock = 0;
			gpu.line++;

			if (gpu.line == 143) {
				gpu.mode = MODE_VBLANK;
				LCD_update();
			} else {
				gpu.mode = MODE_OAM;
			}
		}
		break;

	case MODE_VBLANK:
		if (gpu.mode_clock >= 456) {
			gpu.mode_clock = 0;
			gpu.line++;

			if (gpu.line > 153) {
				gpu.mode = MODE_OAM;
				gpu.line = 0;
			}
		}
		break;
	}
}
