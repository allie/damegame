#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <sdl2/sdl.h>

void Debugger_init();
void Debugger_destroy();
void Debugger_handle_input(SDL_Event event);
void Debugger_update();
void Debugger_draw();

#endif
