#include "lcd.h"
#include "gpu.h"
#include "ui.h"
#include <sdl2/sdl.h>

static SDL_Texture* framebuffer;
extern UIWindow lcd;
extern GPU gpu;

void LCD_init() {
	framebuffer = SDL_CreateTexture(lcd.renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, lcd.base_width, lcd.base_height);
}

void LCD_update() {
	SDL_UpdateTexture(framebuffer, NULL, gpu.screen, 160 * 3);
}

void LCD_draw() {
	SDL_SetRenderDrawColor(lcd.renderer, 255, 255, 255, 255);
	SDL_RenderClear(lcd.renderer);
	SDL_RenderCopy(lcd.renderer, framebuffer, NULL, NULL);
	SDL_RenderPresent(lcd.renderer);
}
