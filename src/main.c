#if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#define OS_WIN
#endif

#ifndef OS_WIN
#include <pthread.h>
#else
#define DWORD WIN_DWORD
#include <windows.h>
#undef DWORD
#define DWORD CGB_DWORD
#endif

#include "common.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "cart.h"
#include "ui.h"
#include <sdl2/sdl.h>

#ifdef OS_WIN
#undef DWORD
#define DWORD ambiguous use CGB_DWORD
#endif

extern CPU cpu;
extern GPU gpu;
extern MMU mmu;

#ifndef OS_WIN
void* emulate(void* vargp) {
#else
WIN_DWORD WINAPI emulate(void* vargp) {
#endif
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

#ifndef OS_WIN
	pthread_t emu_thread;
	pthread_create(&emu_thread, NULL, emulate, NULL);
#else
	HANDLE emu_thread = CreateThread(NULL, 0, emulate, NULL, 0, NULL);
#endif

	UI_loop();
	UI_destroy();

	return 0;
}
