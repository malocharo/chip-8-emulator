#include "cpu.h"
#include "display.h"
#include <stdio.h>

int main()
{
    SDL_Event e;
    init();
    init_display();
    load_game("rom/pong.rom");
    for(;;)
    {
        one_cycle();

        while(SDL_PollEvent(&e))
            sdl_process_event(&e);
        sdl_draw();
    }

}