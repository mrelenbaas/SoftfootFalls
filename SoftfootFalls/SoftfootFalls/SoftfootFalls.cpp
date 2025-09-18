#include <stdio.h>
#include <iostream>


#include "Print.h"
#include "Clock.h"
#include "SoftfootFalls.h"

#if _WIN32
#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>
#elif __linux__
#include "SDL2/SDL.h"
#endif


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main()
{
    Clock* clock = new Clock();
    //SDL_Window* window = NULL;

    printf("Hello World\n");

    delete clock;
    return 0;
}
