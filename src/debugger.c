#include "debugger.h"
#include "ui.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"

UIWindow debugger;
extern CPU cpu;
extern MMU mmu;
extern GPU gpu;

enum {
	TAB_CPU = 0,
	TAB_MMU,
	TAB_GPU,
	TAB_APU,
	TAB_LOG
};
// 　ROM0  0000  00 00 00 00  00 00 00 00
static int tab = 1;
static int mmu_top_row = 0;
static char mmu_str[0x10000 / 8 * 38];

static void Debugger_print_registers() {
	UI_render_string(&debugger, "REGISTERS", 0, 3);
	UI_render_string(&debugger, "B", 0, 5);
	UI_render_string(&debugger, "C", 0, 6);
	UI_render_string(&debugger, "D", 0, 7);
	UI_render_string(&debugger, "E", 0, 8);
	UI_render_string(&debugger, "H", 0, 9);
	UI_render_string(&debugger, "L", 0, 10);
	UI_render_string(&debugger, "A", 0, 11);
	UI_render_string(&debugger, "F", 0, 12);
	UI_render_string(&debugger, "BC", 0, 13);
	UI_render_string(&debugger, "DE", 0, 14);
	UI_render_string(&debugger, "HL", 0, 15);
	UI_render_string(&debugger, "AF", 0, 16);
	UI_render_string(&debugger, "PC", 0, 17);

	UI_render_hex(&debugger, REG_B, 1, 3, 5);
	UI_render_hex(&debugger, REG_C, 1, 3, 6);
	UI_render_hex(&debugger, REG_D, 1, 3, 7);
	UI_render_hex(&debugger, REG_E, 1, 3, 8);
	UI_render_hex(&debugger, REG_H, 1, 3, 9);
	UI_render_hex(&debugger, REG_L, 1, 3, 10);
	UI_render_hex(&debugger, REG_A, 1, 3, 11);
	UI_render_hex(&debugger, REG_F, 1, 3, 12);
	UI_render_hex(&debugger, REG_BC, 2, 3, 13);
	UI_render_hex(&debugger, REG_DE, 2, 3, 14);
	UI_render_hex(&debugger, REG_HL, 2, 3, 15);
	UI_render_hex(&debugger, REG_AF, 2, 3, 16);
	UI_render_hex(&debugger, cpu.pc, 2, 3, 17);

	UI_render_string(&debugger, "FLAGS", 12, 3);
	UI_render_string(&debugger, "Z", 12, 5);
	UI_render_string(&debugger, "N +", 12, 6);
	UI_render_string(&debugger, "H -", 12, 7);
	UI_render_string(&debugger, "C -", 12, 8);

	if (FLAG_Z) {
		UI_render_string(&debugger, "+", 14, 5);
	} else {
		UI_render_string(&debugger, "-", 14, 5);
	}

	if (FLAG_N) {
		UI_render_string(&debugger, "+", 14, 6);
	} else {
		UI_render_string(&debugger, "-", 14, 6);
	}

	if (FLAG_H) {
		UI_render_string(&debugger, "+", 14, 7);
	} else {
		UI_render_string(&debugger, "-", 14, 7);
	}

	if (FLAG_C) {
		UI_render_string(&debugger, "+", 14, 8);
	} else {
		UI_render_string(&debugger, "-", 14, 8);
	}
}

static void Debugger_print_status() {
	UI_render_string(&debugger, "-----------------------------------------------------", 0, 28);
	UI_render_string(&debugger, "STATUS: PAUSED", 0, 29);

	unsigned long frames = 249893;
	UI_render_string(&debugger, "<-", 18, 29);
	UI_render_hex(&debugger, frames, 8, 21, 29);
	UI_render_string(&debugger, "->", 38, 29);
}

static void Debugger_print_mmu() {
	UI_render_string(&debugger, "^", 39, 2);
	UI_render_string(&debugger, "|", 39, 3);
	UI_render_string(&debugger, "|", 39, 26);
	UI_render_string(&debugger, "v", 39, 27);

	for (int i = 0; i < 26; i++) {
		UI_render_string(&debugger, mmu_str + ((mmu_top_row + i) * 38), 0, i + 2);
	}
}

static void Debugger_format_memory_chunk(char* buf, BYTE* data, size_t size, const char* name, int off, int* ptr) {
	int ptr_old = *ptr;
	for (int i = 0; i < size;) {
		snprintf(buf + (*ptr), 6, "%5s", name);
		(*ptr) += 5;
		snprintf(buf + (*ptr), 8, "  %04X ", off + i);
		(*ptr) += 7;

		for (int j = 0; j < 2; j++) {
			snprintf(buf + (*ptr), 2, " ");
			(*ptr) += 1;

			for (int k = 0; k < 4; k++) {
				snprintf(buf + (*ptr), 4, "%02X ", data[i]);
				(*ptr) += 3;
				i++;
			}
		}

		buf[(*ptr) - 1] = '\0';
	}
}

static void Debugger_update_mmu() {
	int ptr = 0;
	Debugger_format_memory_chunk(mmu_str, mmu.rom, 0x4000, "ROM0", 0, &ptr);
	Debugger_format_memory_chunk(mmu_str, mmu.rom + 0x4000, 0x4000, "ROM1", 0x4000, &ptr);
	Debugger_format_memory_chunk(mmu_str, gpu.vram, 0x2000, "VRAM", 0x8000, &ptr);
	Debugger_format_memory_chunk(mmu_str, mmu.eram, 0x2000, "ERAM", 0xA000, &ptr);
	Debugger_format_memory_chunk(mmu_str, mmu.wram, 0x1000, "WRAM0", 0xC000, &ptr);
	Debugger_format_memory_chunk(mmu_str, mmu.wram + 0x1000, 0x1000, "WRAM1", 0xD000, &ptr);
}

static void Debugger_change_tab(int diff) {
	if (diff < 0 && tab > 0) {
		tab--;
	} else if (diff > 0 && tab < 4) {
		tab++;
	}
}

void Debugger_handle_input(SDL_Event event) {
	switch (event.key.keysym.sym) {
	case SDLK_RIGHT:
		if (event.key.keysym.mod == KMOD_LSHIFT) {
			// Frame advance
		} else {
			Debugger_change_tab(1);
		}
		break;
	case SDLK_LEFT:
		if (event.key.keysym.mod == KMOD_LSHIFT) {
			// Frame rewind
		} else {
			Debugger_change_tab(-1);
		}
		break;
	}
}

void Debugger_update() {
	switch (tab) {
	case TAB_CPU:
		break;
	case TAB_MMU:
		Debugger_update_mmu();
		break;
	}
}

void Debugger_draw() {
	SDL_RenderClear(debugger.renderer);

	switch (tab) {
	case TAB_CPU:
		UI_render_string(&debugger, "<CPU> MMU  GPU  APU  LOG", 0, 0);
		UI_render_string(&debugger, "-----------------------------------------------------", 0, 1);
		Debugger_print_registers();
		break;
	case TAB_MMU:
		UI_render_string(&debugger, " CPU <MMU> GPU  APU  LOG", 0, 0);
		UI_render_string(&debugger, "-----------------------------------------------------", 0, 1);
		Debugger_print_mmu();
		break;
	case TAB_GPU:
		UI_render_string(&debugger, " CPU  MMU <GPU> APU  LOG", 0, 0);
		UI_render_string(&debugger, "-----------------------------------------------------", 0, 1);
		break;
	case TAB_APU:
		UI_render_string(&debugger, " CPU  MMU  GPU <APU> LOG", 0, 0);
		UI_render_string(&debugger, "-----------------------------------------------------", 0, 1);
		break;
	case TAB_LOG:
		UI_render_string(&debugger, " CPU  MMU  GPU  APU <LOG>", 0, 0);
		UI_render_string(&debugger, "-----------------------------------------------------", 0, 1);
		break;
	}

	Debugger_print_status();

	SDL_RenderPresent(debugger.renderer);
}
