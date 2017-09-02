#include "common.h"
#include "cpu.h"
#include "mmu.h"
#include "gpu.h"
#include "gui.h"
#include "cart.h"
#include <gtk/gtk.h>
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
	pthread_t tid;
	pthread_create(&tid, NULL, emulate, NULL);

	GtkApplication* app = gtk_application_new("com.cgb", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(GUI_init), NULL);
	// g_timeout_add(16, GUI_refresh_debugger, NULL);

	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
