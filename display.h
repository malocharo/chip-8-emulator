#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL2/SDL.h>
#include "cpu.h"


#define W_WIDTH 1024
#define W_HEIGHT 512


SDL_Window* window;
SDL_Renderer* render;
SDL_Texture* texture;

uint32_t pixels[SCREEN_HEIGHT * SCREEN_WIDTH];

void init_display();
void sdl_process_event(SDL_Event *);
void sdl_draw();

#endif //DISPLAY_H