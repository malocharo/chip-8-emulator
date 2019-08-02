#include "display.h"

uint8_t keymap[NB_KEY] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_a,
    SDLK_z,
    SDLK_e,
    SDLK_q,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};


void init_display()
{
    if(SDL_Init(SDL_INIT_EVERYTHING)< 0)
    {
        fprintf(stderr,"SDL could not initialize, SDL_Error : %s\n",SDL_GetError());
        exit(-1);
    }

    window = SDL_CreateWindow("CHIP-8 Emulator",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,W_WIDTH,W_HEIGHT,SDL_WINDOW_SHOWN);
    if(window == NULL) 
    {
        fprintf(stderr,"Window could not be created. SDL_Error %s\n",SDL_GetError());
        exit(-1);    
    }

    render = SDL_CreateRenderer(window,-1,0);
    SDL_RenderSetLogicalSize(render,W_WIDTH,W_HEIGHT);

    texture = SDL_CreateTexture(render,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING,SCREEN_WIDTH,SCREEN_HEIGHT);
    
}

void sdl_process_event(SDL_Event * e)
{
    if(e->type == SDL_QUIT)
        exit(0);
    if(e->type == SDL_KEYDOWN)
    {
        if(e->key.keysym.sym == SDLK_ESCAPE)
            exit(0);
        
        for(int i = 0;i < NB_KEY;i++)
            if(e->key.keysym.sym == keymap[i])
                key[i] = 1;

        
    }

    if(e->key.keysym.sym == SDL_KEYUP)
    {
        for(int i = 0;i < NB_KEY;i++)
            if(e->key.keysym.sym == keymap[i])
                key[i] = 0;
    }
    
}

void sdl_draw()
{
    if(!draw_flag) return;
    draw_flag = 0;
    // because of sdl we need a Uint32 but we got a Uint8
    for(int i = 0;i < SCREEN_HEIGHT * SCREEN_WIDTH;i++)
        pixels[i]  = (0x00FFFFFF * graph[i]) | 0xFF000000;
    
    SDL_UpdateTexture(texture,NULL,pixels,SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(render);
    SDL_RenderCopy(render,texture,NULL,NULL);
    SDL_RenderPresent(render);
    

}
