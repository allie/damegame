#include <string.h>
#include "mmu.h"
#include "cpu.h"
// #include "gpu.h"

MMU mmu;
extern CPU cpu;
// extern GPU gpu;

static BYTE* MMU_get_ptr(WORD addr) {
	switch (addr & 0xF000) {

	// BIOS / ROM0
	case 0x0000:
		if (mmu.in_bios) {
			if (addr < 0x0100) {
				return mmu.bios + addr;
				break;
			} else if (cpu.pc == 0x0100) {
				mmu.in_bios = 0;
			}
		}
		return mmu.rom + addr;
		break;

	// ROM0
	case 0x1000:
	case 0x2000:
	case 0x3000:
		return mmu.rom + addr;
		break;

	// ROM1 (unbanked)
	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
		return mmu.rom + addr;
		break;

	// VRAM
	case 0x8000:
	case 0x9000:
		// return gpu.vram + (addr & 0x1FFF);
		break;

	// ERAM
	case 0xA000:
	case 0xB000:
		return mmu.eram + (addr & 0x1FFF);
		break;

	// WRAM
	case 0xC000:
	case 0xD000:
		return mmu.wram + (addr & 0x1FFF);
		break;

	// WRAM shadow
	case 0xE000:
		return mmu.wram + (addr & 0x1FFF);
		break;

	// WRAM shadow, IO, ZRAM
	case 0xF000:
		switch (addr & 0x0F00) {

		// WRAM shadow
		case 0x000:
		case 0x100:
		case 0x200:
		case 0x300:
		case 0x400:
		case 0x500:
		case 0x600:
		case 0x700:
		case 0x800:
		case 0x900:
		case 0xA00:
		case 0xB00:
		case 0xC00:
		case 0xD00:
			return mmu.wram + (addr & 0x1FFF);
			break;

		// OAM
		case 0xE00:
			if (addr < 0xFEA0) {
				// return gpu.oam + (addr & 0xFF);
			} else {
				return 0;
			}
			break;

		// ZRAM
		case 0xF00:
			if (addr >= 0xFF80) {
				return mmu.zram + (addr & 0x7F);
			} else {
				// TODO: IO
				return 0;
			}
			break;
		}
		break;
	}

	return 0;
}

void MMU_init() {
	mmu.bios = (BYTE*)calloc(0x100, sizeof(BYTE));
	mmu.rom = (BYTE*)calloc(0x4000, sizeof(BYTE));
	mmu.eram = (BYTE*)calloc(0x2000, sizeof(BYTE));
	mmu.wram = (BYTE*)calloc(0x2000, sizeof(BYTE));
	mmu.zram = (BYTE*)calloc(0x80, sizeof(BYTE));
}

void MMU_load_bios(BYTE* bios) {
	memcpy(mmu.bios, bios, 0x100);
	mmu.in_bios = 1;
}

void MMU_reset() {

}

BYTE MMU_read_8(WORD addr) {
	BYTE* ptr = MMU_get_ptr(addr);
	return ptr ? *ptr : 0;
}

void MMU_write_8(WORD addr, BYTE val) {
	BYTE* ptr = MMU_get_ptr(addr);
	if (ptr) {
		*ptr = val;
	}
}

WORD MMU_read_16(WORD addr) {
	return MMU_read_8(addr) | (MMU_read_8(addr + 1) << 8);
}

void MMU_write_16(WORD addr, WORD val) {
	MMU_write_8(addr, val & 0xFF);
	MMU_write_8(addr + 1, val >> 8);
}
