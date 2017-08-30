#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

void GUI_init(GtkApplication*, gpointer);
void GUI_toggle_debugger();
void GUI_refresh_memory_map();
void GUI_refresh_execution_log();
void GUI_refresh_register_display();

#endif
