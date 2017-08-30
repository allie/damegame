#include "gui.h"
#include "cpu.h"
#include "mmu.h"

extern CPU cpu;
extern MMU mmu;

struct GUI {
	GtkApplication* app;
	gpointer user_data;
	GtkWidget* main_window;
	GtkWidget* debug_window;
	GtkWidget* register_view;
	GtkWidget* execution_log_view;
	GtkWidget* memory_map_view;
	GtkTreeModel* register_model;
	GtkTreeModel* execution_log_model;
	GtkTreeModel* memory_map_model;
	GtkListStore* register_store;
	GtkListStore* execution_log_store;
	GtkListStore* memory_map_store;
};

static struct GUI gui;

void GUI_init(GtkApplication* app, gpointer user_data) {
	gui.app = app;
	gui.user_data = user_data;

	gui.main_window = gtk_application_window_new(gui.app);
	gtk_window_set_title(GTK_WINDOW(gui.main_window), "cgb");
	gtk_window_set_default_size(GTK_WINDOW(gui.main_window), 160, 144);

	// TODO: menu bar

	gui.debug_window = gtk_application_window_new(gui.app);
	gtk_window_set_title(GTK_WINDOW(gui.debug_window), "cgb Debug Tool");
	gtk_window_set_default_size(GTK_WINDOW(gui.debug_window), 640, 480);

	gtk_widget_show_all(gui.debug_window);
	gtk_widget_show_all(gui.main_window);
}

void GUI_toggle_debugger() {
	if (gtk_widget_is_visible(gui.debug_window)) {
		gtk_widget_hide(gui.debug_window);
	} else {
		gtk_widget_show_all(gui.debug_window);
	}
}

void GUI_refresh_memory_map() {

}

void GUI_refresh_execution_log() {

}

void GUI_refresh_register_display() {

}
