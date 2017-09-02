#include "gui.h"
#include "cpu.h"
#include "gpu.h"
#include "mmu.h"

extern CPU cpu;
extern GPU gpu;
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
	GtkTextBuffer* memory_map_buf;
	// char memory_map_buf_raw[0x10000 / 16 * 69 + 1];
	char memory_map_buf_raw[0xFFFFF];
	GtkWidget* refresh_button;
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

	GtkTreeIter iter;
	for (int i = 0; i < 17; i++) {
		gtk_list_store_append(gui.register_store, &iter);
	}

	GUI_refresh_register_display();
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui.register_view), GTK_TREE_MODEL(gui.register_store));

	gtk_widget_set_hexpand(gui.register_view, 1);
	gtk_widget_set_vexpand(gui.register_view, 1);
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

	GUI_refresh_execution_log();

	gtk_widget_set_hexpand(gui.execution_log_view, 1);
	gtk_widget_set_vexpand(gui.execution_log_view, 1);
}

static void GUI_init_memory_map() {
	gui.memory_map_view = gtk_text_view_new();
	gtk_text_view_set_monospace(GTK_TEXT_VIEW(gui.memory_map_view), 1);
	gui.memory_map_buf = gtk_text_buffer_new(NULL);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(gui.memory_map_view), gui.memory_map_buf);

	GUI_refresh_memory_map();

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
	gtk_window_set_default_size(GTK_WINDOW(gui.debug_window), 76, 480);

	gui.debug_grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(gui.debug_window), gui.debug_grid);

	gui.debug_register_scroll = gtk_scrolled_window_new(NULL, NULL);
	gui.debug_execution_log_scroll = gtk_scrolled_window_new(NULL, NULL);
	gui.debug_memory_map_scroll = gtk_scrolled_window_new(NULL, NULL);

	gui.refresh_button = gtk_button_new_with_label("Refresh");
	gtk_widget_set_hexpand(gui.refresh_button, 1);
	gtk_widget_set_vexpand(gui.refresh_button, 1);
	g_signal_connect(
		gui.refresh_button,
		"clicked",
		G_CALLBACK(GUI_refresh_debugger),
		NULL
	);

	gtk_grid_attach(GTK_GRID(gui.debug_grid), gui.debug_register_scroll, 1, 1, 1, 6);
	// gtk_grid_attach(GTK_GRID(gui.debug_grid), gui.debug_execution_log_scroll, 2, 1, 5, 1);
	gtk_grid_attach(GTK_GRID(gui.debug_grid), gui.debug_memory_map_scroll, 2, 1, 4, 8);
	gtk_grid_attach(GTK_GRID(gui.debug_grid), gui.refresh_button, 1, 7, 1, 1);

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

static void GUI_format_memory_chunk(char* buf, BYTE* data, size_t size, const char* name, int off, int* ptr) {
	for (int i = 0; i < size;) {
		snprintf(buf + (*ptr), 6, "%5s", name);
		(*ptr) += 5;
		snprintf(buf + (*ptr), 8, "   %04X", off + i);
		(*ptr) += 7;

		for (int j = 0; j < 4; j++) {
			snprintf(buf + (*ptr), 4, "   ");
			(*ptr) += 3;

			for (int k = 0; k < 4; k++) {
				snprintf(buf + (*ptr), 4, "%02X ", data[i]);
				(*ptr) += 3;
				i++;
			}
		}

		snprintf(buf + (*ptr), 2, "\n");
		(*ptr)++;
	}
}

void GUI_refresh_memory_map() {
	int ptr = 0;
	// GUI_format_memory_chunk(gui.memory_map_buf_raw, mmu.rom, 0x4000, "ROM0", 0, &ptr);
	// GUI_format_memory_chunk(gui.memory_map_buf_raw, mmu.rom + 0x4000, 0x4000, "ROM1", 0x4000, &ptr);
	// GUI_format_memory_chunk(gui.memory_map_buf_raw, gpu.vram, 0x2000, "VRAM", 0x8000, &ptr);
	// GUI_format_memory_chunk(gui.memory_map_buf_raw, mmu.eram, 0x2000, "ERAM", 0xA000, &ptr);
	// GUI_format_memory_chunk(gui.memory_map_buf_raw, mmu.wram, 0x1000, "WRAM0", 0xC000, &ptr);
	// GUI_format_memory_chunk(gui.memory_map_buf_raw, mmu.wram + 0x1000, 0x1000, "WRAM1", 0xD000, &ptr);
	GUI_format_memory_chunk(gui.memory_map_buf_raw, gpu.screen, 160 * 144 * 4, "SCRN", 0, &ptr);

	gtk_text_buffer_set_text(gui.memory_map_buf, gui.memory_map_buf_raw, -1);
}

void GUI_refresh_execution_log() {
	gtk_tree_view_set_model(GTK_TREE_VIEW(gui.execution_log_view), NULL);

	// TODO

	gtk_tree_view_set_model(GTK_TREE_VIEW(gui.execution_log_view), GTK_TREE_MODEL(gui.execution_log_store));
}

void GUI_refresh_register_display() {
	GtkTreeIter iter;
	gtk_tree_model_get_iter_first(GTK_TREE_MODEL(gui.register_store), &iter);

	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "B",
		COL_REG_VAL, REG_B,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "C",
		COL_REG_VAL, REG_C,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "D",
		COL_REG_VAL, REG_D,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "E",
		COL_REG_VAL, REG_E,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "H",
		COL_REG_VAL, REG_H,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "L",
		COL_REG_VAL, REG_L,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "A",
		COL_REG_VAL, REG_A,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "F",
		COL_REG_VAL, REG_F,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "BC",
		COL_REG_VAL, REG_BC,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "DE",
		COL_REG_VAL, REG_DE,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "HL",
		COL_REG_VAL, REG_HL,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "AF",
		COL_REG_VAL, REG_AF,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "PC",
		COL_REG_VAL, REG_PC,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag Z",
		COL_REG_VAL, FLAG_Z,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag N",
		COL_REG_VAL, FLAG_N,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag H",
		COL_REG_VAL, FLAG_H,
		-1
	);

	gtk_tree_model_iter_next(GTK_TREE_MODEL(gui.register_store), &iter);
	gtk_list_store_set(
		gui.register_store, &iter,
		COL_REG_NAME, "Flag C",
		COL_REG_VAL, FLAG_C,
		-1
	);
}

gboolean GUI_refresh_debugger(gpointer data) {
	GUI_refresh_register_display();
	GUI_refresh_memory_map();
	GUI_refresh_execution_log();
	return 1;
}
