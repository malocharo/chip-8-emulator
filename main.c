#include "cpu.h"

#include <stdio.h>

int main()
{
    init();
    load_game("rom/pong.rom");
    for(;;)
        one_cycle();
}