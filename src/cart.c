#include "cart.h"
#include "mmu.h"
#include <stdio.h>

extern MMU mmu;

void Cart_load_from_file(const char* path) {
	FILE* fp = fopen(path, "rb");
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);

	fread(mmu.rom, size, sizeof(BYTE), fp);
	// if (size == 0x4000) {
	// 	fread(mmu.rom, size, sizeof(BYTE), fp);
	// } else {
	// 	// ROM fail
	// }

	fclose(fp);
}
