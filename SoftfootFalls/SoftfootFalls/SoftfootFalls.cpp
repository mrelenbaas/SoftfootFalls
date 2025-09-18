#include <stdio.h>
#include <iostream>


#include "Print.h"
#include "Clock.h"
#include "SoftfootFalls.h"

#include <SDL3/SDL.h>
//#include <SDL3/SDL_main.h>

#if _WIN32
#elif __linux__
#endif


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main()
{
    Clock* clock = new Clock();

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
		window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		if (window == NULL)
		{
			SDL_Log(
				"Window could not be created! SDL_Error: %s\n",
				SDL_GetError());
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
			SDL_FillSurfaceRect(screenSurface, NULL, SDL_MapSurfaceRGB(screenSurface, 0xFF, 0xFF, 0xFF));
			SDL_UpdateWindowSurface(window);

			SDL_Event e;
			bool quit = false;
			while (quit == false)
			{
				while (SDL_PollEvent(&e))
				{
					if (e.type == SDL_EVENT_QUIT) quit = true;
				}
			}
		}
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    delete clock;
    return 0;
}
