#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <functional>
#include <string>

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
#include "Load.h"

#ifdef _WIN32
#elif __linux__
#endif


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

bool init();
bool loadMedia();
void close();
SDL_Surface* loadSurface(const char* path, Load* load, bool* success);

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface* gCurrentSurface = NULL;

bool init()
{
	bool success = true;

#ifdef _WIN32
	if (!SDL_Init(SDL_INIT_VIDEO))
#elif __linux__
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
#endif
	{
		SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
#ifdef _WIN32
		gWindow = SDL_CreateWindow("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
#elif __linux__
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
#endif
		if (gWindow == NULL)
		{
			SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}

bool loadMedia()
{
	using namespace std;

	Load* load = new Load(SDL_GetBasePath());
	bool success = true;

	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface(load->Path("press.bmp"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface(load->Path("up.bmp"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface(load->Path("down.bmp"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface(load->Path("left.bmp"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface(load->Path("right.bmp"), load, &success);
	
	delete load;
	return success;
}

void close()
{
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
	{
#ifdef _WIN32
		SDL_DestroySurface(gKeyPressSurfaces[i]);
#elif __linux__
		SDL_FreeSurface(gKeyPressSurfaces[i]);
#endif
		gKeyPressSurfaces[i] = NULL;
	}

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
}

SDL_Surface* loadSurface(const char* path, Load* load, bool* success)
{
	SDL_Surface* loadedSurface = SDL_LoadBMP(path);
	load->Print(loadedSurface, path);
	if (loadedSurface == NULL)
	{
		SDL_Log("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
		(*success) = false;
	}

	return loadedSurface;
}

int main(int argc, char* argv[])
{
    Clock* clock = new Clock();
	Timer* timer = new Timer(Timer::Print, 1);

	if (!init())
	{
		SDL_Log("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia())
		{
			SDL_Log("Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			SDL_Event e;
			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
			while (!quit)
			{
				timer->Update();
				while (SDL_PollEvent(&e) != 0)
				{
#ifdef _WIN32
					if (e.type == SDL_EVENT_QUIT)
#elif __linux__
					if (e.type == SDL_QUIT)
#endif
						quit = true;
					else if (e.key.key)
					{
						switch (e.key.key)
						{
						case SDLK_UP:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
							break;
						case SDLK_DOWN:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
							break;
						case SDLK_LEFT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
							break;
						case SDLK_RIGHT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
							break;
						default:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
							break;
						}
					}
				}
				SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}
	
	close();
	delete timer;
    delete clock;
    return 0;
}
