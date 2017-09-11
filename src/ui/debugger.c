#include "debugger.h"
#include "disassembler.h"
#include "ui.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include <sdl2/sdl.h>

extern UIWindow debugger;
extern CPU cpu;
extern MMU mmu;
extern GPU gpu;
extern CPU_history cpu_history;
SDL_mutex* debugger_mutex;

enum {
	TAB_CPU = 0,
	TAB_MMU,
	TAB_GPU,
	TAB_APU
};

static int tab = 0;
static int mmu_top_row = 0;
static char mmu_str[0x10000 / 8 * 38];
static char ins_str[40 * 26];
static FILE* logfile;

static void Debugger_print_registers(int x, int y) {
	UI_render_string(&debugger, "REGISTERS", x, y);
	UI_render_string(&debugger, "B  0x", x, y + 2);
	UI_render_string(&debugger, "C  0x", x, y + 3);
	UI_render_string(&debugger, "D  0x", x, y + 4);
	UI_render_string(&debugger, "E  0x", x, y + 5);
	UI_render_string(&debugger, "H  0x", x, y + 6);
	UI_render_string(&debugger, "L  0x", x, y + 7);
	UI_render_string(&debugger, "A  0x", x, y + 8);
	UI_render_string(&debugger, "F  0x", x, y + 9);
	UI_render_string(&debugger, "BC 0x", x, y + 10);
	UI_render_string(&debugger, "DE 0x", x, y + 11);
	UI_render_string(&debugger, "HL 0x", x, y + 12);
	UI_render_string(&debugger, "AF 0x", x, y + 13);
	UI_render_string(&debugger, "PC 0x", x, y + 14);

	UI_render_hex(&debugger, REG_B, 1, x + 5, y + 2);
	UI_render_hex(&debugger, REG_C, 1, x + 5, y + 3);
	UI_render_hex(&debugger, REG_D, 1, x + 5, y + 4);
	UI_render_hex(&debugger, REG_E, 1, x + 5, y + 5);
	UI_render_hex(&debugger, REG_H, 1, x + 5, y + 6);
	UI_render_hex(&debugger, REG_L, 1, x + 5, y + 7);
	UI_render_hex(&debugger, REG_A, 1, x + 5, y + 8);
	UI_render_hex(&debugger, REG_F, 1, x + 5, y + 9);
	UI_render_hex(&debugger, REG_BC, 2, x + 5, y + 10);
	UI_render_hex(&debugger, REG_DE, 2, x + 5, y + 11);
	UI_render_hex(&debugger, REG_HL, 2, x + 5, y + 12);
	UI_render_hex(&debugger, REG_AF, 2, x + 5, y + 13);
	UI_render_hex(&debugger, cpu.pc, 2, x + 5, y + 14);

	UI_render_string(&debugger, "FLAGS", x + 12, y);
	UI_render_string(&debugger, "Z", x + 12, y + 2);
	UI_render_string(&debugger, "N", x + 12, y + 3);
	UI_render_string(&debugger, "H", x + 12, y + 4);
	UI_render_string(&debugger, "C", x + 12, y + 5);

	if (FLAG_Z) {
		UI_render_string(&debugger, "+", x + 14, y + 2);
	} else {
		UI_render_string(&debugger, "-", x + 14, y + 2);
	}

	if (FLAG_N) {
		UI_render_string(&debugger, "+", x + 14, y + 3);
	} else {
		UI_render_string(&debugger, "-", x + 14, y + 3);
	}

	if (FLAG_H) {
		UI_render_string(&debugger, "+", x + 14, y + 4);
	} else {
		UI_render_string(&debugger, "-", x + 14, y + 4);
	}

	if (FLAG_C) {
		UI_render_string(&debugger, "+", x + 14, y + 5);
	} else {
		UI_render_string(&debugger, "-", x + 14, y + 5);
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

static void Debugger_print_log() {
	SDL_LockMutex(debugger_mutex);
	for (int i = 0; i < 26; i++) {
		UI_render_string(&debugger, ins_str + (i * 40), 0, 27 - i);
	}
	SDL_UnlockMutex(debugger_mutex);
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

static void Debugger_update_cpu() {
	char buf[40 * 26];
	for (int i = 0; i < 26; i++) {
		DWORD history_addr = cpu_history.ptr < i + 1 ? 0xFFFF - i : cpu_history.ptr - i - 1;
		Disassembler_print(cpu_history.states[history_addr], buf + (i * 40), 40);
	}

	fprintf(logfile, "%s\n", buf);

	SDL_LockMutex(debugger_mutex);
	memcpy(ins_str, buf, 40 * 26);
	SDL_UnlockMutex(debugger_mutex);
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

void Debugger_init() {
	debugger_mutex = SDL_CreateMutex();
	logfile = fopen("log", "w");
	if (logfile == NULL) {
		printf("Unable to open debug log file for writing.\n");
	}
}

void Debugger_destroy() {
	fclose(logfile);
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
		Debugger_update_cpu();
		break;
	case TAB_MMU:
		Debugger_update_mmu();
		break;
	}
}

void Debugger_draw() {
	SDL_SetRenderDrawColor(debugger.renderer, 0, 0, 0, 255);
	SDL_RenderClear(debugger.renderer);

	switch (tab) {
	case TAB_CPU:
		UI_render_string(&debugger, "<CPU> MMU  GPU  APU  LOG", 0, 0);
		UI_render_string(&debugger, "-----------------------------------------------------", 0, 1);
		Debugger_print_log();
		Debugger_print_registers(23, 2);
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
	}

	Debugger_print_status();

	SDL_RenderPresent(debugger.renderer);
}
