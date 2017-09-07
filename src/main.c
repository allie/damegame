#include "common.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "cart.h"
#include <pthread.h>
#include <sdl2/sdl.h>

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
	SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("cgb", 100, 100, 160, 144, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		printf("SDL_CreateWindow error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	// pthread_t tid;
	// pthread_create(&tid, NULL, emulate, NULL);

#define DEFAULT_W 160
#define DEFAULT_H 144

	int ww = 160;
	int wh = 144;

	int running = 1;
	while (running) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				running = 0;
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
				case SDLK_EQUALS:
					if (ww < 1600 && wh < 1440) {
						ww += DEFAULT_W;
						wh += DEFAULT_H;
						SDL_SetWindowSize(window, ww, wh);
					}
					break;
				case SDLK_MINUS:
					if (ww > 160 && wh > 140) {
						ww -= DEFAULT_W;
						wh -= DEFAULT_H;
						SDL_SetWindowSize(window, ww, wh);
					}
					break;
				}
				break;
			}
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
