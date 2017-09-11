#include "ui.h"
#include "lcd.h"
#include "debugger.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

UIWindow debugger;
UIWindow lcd;
UIWindow options;
UIWindow* active;

#include "font.i"

static void UI_scale(UIWindow* ui_window, int delta) {
	if ((delta < 0) && (ui_window->scale > 1)) {
		ui_window->scale--;
	} else if (delta > 0) {
		ui_window->scale++;
	}

	SDL_SetWindowSize(ui_window->window, ui_window->scale * ui_window->base_width, ui_window->scale * ui_window->base_height);
}

void UI_init() {
	lcd.visible = 1;
	lcd.base_width = 160;
	lcd.base_height = 144;
	lcd.scale = 1;
	lcd.window = SDL_CreateWindow("cgb", 100, 100, lcd.base_width, lcd.base_height, SDL_WINDOW_SHOWN);
	if (lcd.window == NULL) {
		printf("SDL_CreateWindow error: %s\n", SDL_GetError());
		SDL_Quit();
	}

	lcd.renderer = SDL_CreateRenderer(lcd.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (lcd.renderer == NULL) {
		SDL_DestroyWindow(lcd.window);
		printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
		SDL_Quit();
	}

	SDL_SetRenderDrawColor(lcd.renderer, 0, 0, 0, 255);
	SDL_RenderSetLogicalSize(lcd.renderer, lcd.base_width, lcd.base_height);

	lcd.font = SDL_CreateTexture(lcd.renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 128, 128);
	SDL_UpdateTexture(lcd.font, NULL, font_tex, 128 * 3);

	debugger.visible = 1;
	debugger.base_width = 320;
	debugger.base_height = 240;
	debugger.scale = 1;
	debugger.window = SDL_CreateWindow("cgb Debugger", 276, 100, debugger.base_width, debugger.base_height, SDL_WINDOW_SHOWN);
	if (debugger.window == NULL) {
		printf("SDL_CreateWindow error: %s\n", SDL_GetError());
		SDL_Quit();
	}

	debugger.renderer = SDL_CreateRenderer(debugger.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (debugger.renderer == NULL) {
		SDL_DestroyWindow(debugger.window);
		printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
		SDL_Quit();
	}

	SDL_SetRenderDrawColor(debugger.renderer, 0, 0, 0, 255);
	SDL_RenderSetLogicalSize(debugger.renderer, debugger.base_width, debugger.base_height);

	debugger.font = SDL_CreateTexture(debugger.renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 128, 128);
	SDL_UpdateTexture(debugger.font, NULL, (void*)font_tex, 128 * 3);

	options.visible = 0;
	options.base_width = 320;
	options.base_height = 240;
	options.scale = 1;
	options.window = SDL_CreateWindow("cgb Options", 612, 100, options.base_width, options.base_height, SDL_WINDOW_HIDDEN);
	if (options.window == NULL) {
		printf("SDL_CreateWindow error: %s\n", SDL_GetError());
		SDL_Quit();
	}

	options.renderer = SDL_CreateRenderer(options.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (options.renderer == NULL) {
		SDL_DestroyWindow(options.window);
		printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
		SDL_Quit();
	}

	SDL_SetRenderDrawColor(options.renderer, 0, 0, 0, 255);
	SDL_RenderSetLogicalSize(options.renderer, options.base_width, options.base_height);

	options.font = SDL_CreateTexture(options.renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STATIC, 128, 128);
	SDL_UpdateTexture(options.font, NULL, font_tex, 128 * 3);

	LCD_init();
	Debugger_init();
}

void UI_destroy() {
	SDL_DestroyWindow(lcd.window);
	SDL_DestroyWindow(debugger.window);
	SDL_DestroyWindow(options.window);

	SDL_DestroyRenderer(lcd.renderer);
	SDL_DestroyRenderer(debugger.renderer);
	SDL_DestroyRenderer(options.renderer);

	SDL_DestroyTexture(lcd.font);
	SDL_DestroyTexture(debugger.font);
	SDL_DestroyTexture(options.font);

	SDL_Quit();
}

void UI_render_string(UIWindow* ui_window, const char* text, unsigned x, unsigned y) {
	for (int i = 0; i < strlen(text); i++) {
		char c = text[i];
		int tx = c % 16;
		int ty = c / 16;
		SDL_Rect src = {tx * 8, ty * 8, 8, 8};
		SDL_Rect dst = {x * 8 + i * 8, y * 8, 8, 8};
		SDL_RenderCopy(ui_window->renderer, ui_window->font, &src, &dst);
	}
}

void UI_render_hex(UIWindow* ui_window, unsigned long val, unsigned bytes, unsigned x, unsigned y) {
	char hex[17];
	snprintf(hex, bytes * 2 + 1, "%0*lX", bytes * 2, val);
	UI_render_string(ui_window, hex, x, y);
}

void UI_toggle(UIWindow* ui_window) {
	if (ui_window->visible) {
		ui_window->visible = 0;
		SDL_HideWindow(ui_window->window);
	} else {
		ui_window->visible = 1;
		SDL_ShowWindow(ui_window->window);
	}
}

void UI_update() {
	// Determine active window
	if (SDL_GetWindowFlags(lcd.window) & SDL_WINDOW_INPUT_FOCUS) {
		active = &lcd;
	} else if (SDL_GetWindowFlags(debugger.window) & SDL_WINDOW_INPUT_FOCUS) {
		active = &debugger;
	} else if (SDL_GetWindowFlags(options.window) & SDL_WINDOW_INPUT_FOCUS) {
		active = &options;
	} else {
		active = NULL;
	}
}

void UI_draw() {
	if (lcd.visible) {
		LCD_draw();
	}

	if (debugger.visible) {
		Debugger_draw();
	}

	if (options.visible) {
		// Options_draw();
	}
}
