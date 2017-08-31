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
	GtkWidget* debug_grid;
	GtkWidget* debug_register_scroll;
	GtkWidget* debug_execution_log_scroll;
	GtkWidget* debug_memory_map_scroll;
	GtkWidget* register_view;
	GtkWidget* execution_log_view;
	GtkWidget* memory_map_view;
	GtkListStore* register_store;
	GtkListStore* execution_log_store;
	GtkListStore* memory_map_store;
};

enum {
	COL_REG_NAME = 0,
	COL_REG_VAL
};

static struct GUI gui;

static void GUI_init_register_display() {
	gui.register_store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_UINT);
	gui.register_view = gtk_tree_view_new();
	// gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gui.register_view), 0);

	GtkCellRenderer* renderer;
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(gui.register_view),
		-1,
		"Register",
		renderer,
		"text",
		COL_REG_NAME,
		NULL
	);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(gui.register_view),
		-1,
		"Value",
		renderer,
		"text",
		COL_REG_VAL,
		NULL
	);

	gtk_tree_view_set_model(GTK_TREE_VIEW(gui.register_view), GTK_TREE_MODEL(gui.register_store));
	gtk_widget_set_hexpand(gui.register_view, 1);
	gtk_widget_set_vexpand(gui.register_view, 1);

	GtkTreeIter iter;
	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "B",
		COL_REG_VAL, REG_B,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "C",
		COL_REG_VAL, REG_C,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "D",
		COL_REG_VAL, REG_D,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "E",
		COL_REG_VAL, REG_E,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "H",
		COL_REG_VAL, REG_H,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "L",
		COL_REG_VAL, REG_L,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "A",
		COL_REG_VAL, REG_A,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "F",
		COL_REG_VAL, REG_F,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "BC",
		COL_REG_VAL, REG_BC,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "DE",
		COL_REG_VAL, REG_DE,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "HL",
		COL_REG_VAL, REG_HL,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "AF",
		COL_REG_VAL, REG_AF,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "PC",
		COL_REG_VAL, REG_PC,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag Z",
		COL_REG_VAL, FLAG_Z,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag N",
		COL_REG_VAL, FLAG_N,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag H",
		COL_REG_VAL, FLAG_H,
		-1
	);

	gtk_list_store_append(gui.register_store, &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag C",
		COL_REG_VAL, FLAG_C,
		-1
	);
}

static void GUI_init_execution_log() {
	gui.execution_log_store = gtk_list_store_new(1, G_TYPE_STRING); // TODO: change
	gui.execution_log_view = gtk_tree_view_new();
	// gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gui.execution_log_view), 0);

	GtkCellRenderer* renderer;
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(gui.execution_log_view),
		-1,
		"Command",
		renderer,
		"text",
		0,
		NULL
	);

	gtk_tree_view_set_model(GTK_TREE_VIEW(gui.execution_log_view), GTK_TREE_MODEL(gui.execution_log_store));
	gtk_widget_set_hexpand(gui.execution_log_view, 1);
	gtk_widget_set_vexpand(gui.execution_log_view, 1);
}

static void GUI_init_memory_map() {
	gui.memory_map_store = gtk_list_store_new(
		18,
		G_TYPE_STRING, /* Memory map section */
		G_TYPE_STRING, /* Memory offset */
		G_TYPE_STRING, /* 16 bytes of memory */
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING
	);

	gui.memory_map_view = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(gui.memory_map_view), 0);

	GtkCellRenderer* renderer;

	for (int i = 0; i < 18; i++) {
		renderer = gtk_cell_renderer_text_new();
		gtk_tree_view_insert_column_with_attributes(
			GTK_TREE_VIEW(gui.memory_map_view),
			-1,
			"",
			renderer,
			"text",
			i,
			NULL
		);
	}

	GtkTreeIter iter;
	for (int i = 0; i < (0x8000 / 16); i += 16) {
		gtk_list_store_append(gui.memory_map_store, &iter);
		gtk_list_store_set(
			gui.memory_map_store, &iter,
			0, "ROM0",
			-1
		);

		char row_addr[5];
		snprintf(row_addr, 5, "%04X", i);
		gtk_list_store_set(
			gui.memory_map_store, &iter,
			1, row_addr,
			-1
		);

		for (int j = 0; j < 16; j++) {
			char val[3];
			snprintf(val, 3, "%02X", mmu.rom[i + j]);
			gtk_list_store_set(
				gui.memory_map_store, &iter,
				j + 2, val,
				-1
			);
		}
	}

	gtk_tree_view_set_model(GTK_TREE_VIEW(gui.memory_map_view), GTK_TREE_MODEL(gui.memory_map_store));
	gtk_widget_set_hexpand(gui.memory_map_view, 1);
	gtk_widget_set_vexpand(gui.memory_map_view, 1);
}

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

	gui.debug_grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(gui.debug_window), gui.debug_grid);

	gui.debug_register_scroll = gtk_scrolled_window_new(NULL, NULL);
	gui.debug_execution_log_scroll = gtk_scrolled_window_new(NULL, NULL);
	gui.debug_memory_map_scroll = gtk_scrolled_window_new(NULL, NULL);

	gtk_grid_attach(GTK_GRID(gui.debug_grid), gui.debug_register_scroll, 1, 1, 1, 2);
	// gtk_grid_attach(GTK_GRID(gui.debug_grid), gui.debug_execution_log_scroll, 2, 1, 5, 1);
	gtk_grid_attach(GTK_GRID(gui.debug_grid), gui.debug_memory_map_scroll, 2, 2, 5, 1);

	GUI_init_register_display();
	GUI_init_execution_log();
	GUI_init_memory_map();

	gtk_container_add(GTK_CONTAINER(gui.debug_register_scroll), gui.register_view);
	gtk_container_add(GTK_CONTAINER(gui.debug_execution_log_scroll), gui.execution_log_view);
	gtk_container_add(GTK_CONTAINER(gui.debug_memory_map_scroll), gui.memory_map_view);

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
