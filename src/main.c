#include "common.h"
#include "cpu.h"

extern CPU cpu;
extern MMU mmu;

int main() {
	MMU_init();
	CPU_reset();

	return 0;
}
