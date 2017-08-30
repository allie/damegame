#include "common.h"
#include "cpu.h"
#include "mmu.h"
#include "gui.h"
#include <gtk/gtk.h>

extern CPU cpu;
extern MMU mmu;

int main(int argc, char** argv) {
	MMU_init();
	CPU_reset();

	GtkApplication* app = gtk_application_new("com.cgb", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(GUI_init), NULL);
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}
