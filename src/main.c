#include "common.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "cart.h"
#include "ui.h"
#include <sdl2/sdl.h>
#include <pthread.h>

extern CPU cpu;
extern GPU gpu;
extern MMU mmu;

void* emulate(void* vargp) {
	MMU_init();
	CPU_reset();
	GPU_init();

	Cart_load_from_file("test.gb");

	while(2) {
		CPU_step();
		GPU_step();
	}
}

int main(int argc, char** argv) {
	MMU_init();
	CPU_reset();
	GPU_init();

	UI_init();
	pthread_t emu_thread;
	pthread_create(&emu_thread, NULL, emulate, NULL);
	UI_loop();
	UI_destroy();

	return 0;
}
