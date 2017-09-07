#ifndef UI_H
#define UI_H

#include <sdl2/sdl.h>

typedef struct {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* font;
	int base_width;
	int base_height;
	int scale;
	int visible;
} UIWindow;

void UI_init();
void UI_destroy();
void UI_render_string(UIWindow*, const char*, unsigned, unsigned);
void UI_render_hex(UIWindow*, unsigned long, unsigned, unsigned, unsigned);
void UI_toggle(UIWindow*);
void UI_loop();

#endif
