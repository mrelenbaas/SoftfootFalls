#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <functional>


#include "Print.h"
#include "Time.h"
#include "Clock.h"
#include "Timer.h"
#include "SoftfootFalls.h"

#ifdef _WIN32
#include <SDL3/SDL.h>
#elif __linux__
#include <SDL2/SDL.h>
#endif

#ifdef _WIN32
#elif __linux__
#endif


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* argv[])
{
    Clock* clock = new Clock();
	Timer* timer = new Timer(Timer::Print, 1);

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
#ifdef _WIN32
    if (!SDL_Init(SDL_INIT_VIDEO))
#elif __linux__
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
#endif
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
#ifdef _WIN32
		window = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
#elif __linux__
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,  SDL_WINDOW_SHOWN);
#endif
		if (window == NULL)
		{
			SDL_Log(
				"Window could not be created! SDL_Error: %s\n",
				SDL_GetError());
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
#ifdef _WIN32
			SDL_FillSurfaceRect(screenSurface, NULL, SDL_MapSurfaceRGB(screenSurface, 0xFF, 0xFF, 0xFF));
#elif __linux__
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
#endif
			SDL_UpdateWindowSurface(window);

#ifdef _WIN32
			SDL_Event e;
			bool quit = false;
			while (quit == false)
			{
				timer->Update();
				while (SDL_PollEvent(&e))
				{
					if (e.type == SDL_EVENT_QUIT) quit = true;
				}
			}
#elif __linux__
			SDL_Event e; bool quit = false; while (quit == false) {while(SDL_PollEvent(&e)) {if(e.type == SDL_QUIT) quit = true;}}
#endif
		}
    }
    screenSurface = NULL;
    SDL_DestroyWindow(window);
    SDL_Quit();
	delete timer;
    delete clock;
    return 0;
}
