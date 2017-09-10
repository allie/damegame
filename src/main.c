#include "common.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "cart.h"
#include "ui.h"
#include <sdl2/sdl.h>

SDL_Thread* emu_thread;
extern CPU cpu;
extern GPU gpu;
extern MMU mmu;

static int emulate(void* data) {
	Cart_load_from_file("test.gb");

	while(2) {
		CPU_step();
		GPU_step();
	}
}

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
	}

	MMU_init();
	CPU_reset();
	GPU_init();
	UI_init();

	emu_thread = SDL_CreateThread(emulate, "hardware", (void*)NULL);

	UI_loop();
	UI_destroy();

	return 0;
}
