#include "input.h"
#include "config.h"
#include <stdlib.h>

extern Config config;

void Input_handle(SDL_Keysym sym) {
	// Gameplay inputs
	if (sym.sym == config.keys.a.sym && sym.mod == config.keys.a.mod) {

	}

	else if (sym.sym == config.keys.b.sym && sym.mod == config.keys.b.mod) {

	}

	else if (sym.sym == config.keys.start.sym && sym.mod == config.keys.start.mod) {

	}

	else if (sym.sym == config.keys.select.sym && sym.mod == config.keys.select.mod) {

	}

	else if (sym.sym == config.keys.reset.sym && sym.mod == config.keys.reset.mod) {

	}

	// Save state inputs
	else if (sym.sym == config.keys.change_state_1.sym && sym.mod == config.keys.change_state_1.mod) {

	}

	else if (sym.sym == config.keys.change_state_2.sym && sym.mod == config.keys.change_state_2.mod) {

	}

	else if (sym.sym == config.keys.change_state_3.sym && sym.mod == config.keys.change_state_3.mod) {

	}

	else if (sym.sym == config.keys.change_state_4.sym && sym.mod == config.keys.change_state_4.mod) {

	}

	else if (sym.sym == config.keys.change_state_5.sym && sym.mod == config.keys.change_state_5.mod) {

	}

	else if (sym.sym == config.keys.change_state_6.sym && sym.mod == config.keys.change_state_6.mod) {

	}

	else if (sym.sym == config.keys.change_state_7.sym && sym.mod == config.keys.change_state_7.mod) {

	}

	else if (sym.sym == config.keys.change_state_8.sym && sym.mod == config.keys.change_state_8.mod) {

	}

	else if (sym.sym == config.keys.change_state_9.sym && sym.mod == config.keys.change_state_8.mod) {

	}

	else if (sym.sym == config.keys.save_state.sym && sym.mod == config.keys.save_state.mod) {

	}

	else if (sym.sym == config.keys.load_state.sym && sym.mod == config.keys.load_state.mod) {

	}

	// UI inputs
	else if (sym.sym == config.keys.ui_scale_up.sym && sym.mod == config.keys.ui_scale_up.mod) {

	}

	else if (sym.sym == config.keys.ui_scale_down.sym && sym.mod == config.keys.ui_scale_down.mod) {

	}

	else if (sym.sym == config.keys.ui_tab_right.sym && sym.mod == config.keys.ui_tab_right.mod) {

	}

	else if (sym.sym == config.keys.ui_tab_left.sym && sym.mod == config.keys.ui_tab_left.mod) {

	}

	else if (sym.sym == config.keys.ui_scroll_down.sym && sym.mod == config.keys.ui_scroll_down.mod) {

	}

	else if (sym.sym == config.keys.ui_scroll_up.sym && sym.mod == config.keys.ui_scroll_up.mod) {

	}

	else if (sym.sym == config.keys.ui_page_flip.sym && sym.mod == config.keys.ui_page_flip.mod) {

	}

	// Debugger inputs
	else if (sym.sym == config.keys.debugger_frame_advance.sym && sym.mod == config.keys.debugger_frame_advance.mod) {

	}

	else if (sym.sym == config.keys.debugger_pause.sym && sym.mod == config.keys.debugger_pause.mod) {

	}
}
