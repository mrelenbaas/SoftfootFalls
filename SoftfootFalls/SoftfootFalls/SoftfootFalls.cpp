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
#include <SDL3_image/SDL_image.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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
SDL_Texture* loadTexture(const char* path);

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface* gCurrentSurface = NULL;
SDL_Texture* gTexture = NULL;

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
		if (!SDL_CreateWindowAndRenderer("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &gWindow, &gRenderer))
#elif __linux__
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
#endif
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

	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface(load->Path("press.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface(load->Path("up.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface(load->Path("down.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface(load->Path("left.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface(load->Path("right.png"), load, &success);
	gTexture = loadTexture(load->Path("texture.png"));
	if (gTexture == NULL)
	{
		SDL_Log("Failed to load texture image!\n");
		success = false;
	}

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
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	SDL_Quit();
}

SDL_Surface* loadSurface(const char* path, Load* load, bool* success)
{
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	load->Print(loadedSurface, path);
	if (loadedSurface == NULL)
	{
		SDL_Log("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
		(*success) = false;
	}
	else
	{
#ifdef _WIN32
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format);
#elif __linux__
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
#endif
		if (optimizedSurface == NULL)
		{
			SDL_Log("Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError());
		}
#ifdef _WIN32
		SDL_DestroySurface(loadedSurface);
#elif __linux__
		SDL_FreeSurface(loadedSurface);
#endif
	}

	return optimizedSurface;
}

SDL_Texture* loadTexture(const char* path)
{
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		SDL_Log("Unable to load image %s! SDL_image: %s\n", path, SDL_GetError());

	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			SDL_Log("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}
		SDL_DestroySurface(loadedSurface);
	}

	return newTexture;
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
#ifdef _WIN32
					else if (e.key.key)
#elif __linux__
					else if (e.type == SDL_KEYDOWN)
#endif
					{
#ifdef _WIN32
						switch (e.key.key)
#elif __linux__
						switch (e.key.keysym.sym)
#endif
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
							//gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
							gCurrentSurface = NULL;
							break;
						}
					}
				}
				SDL_RenderClear(gRenderer);
				SDL_Rect stretchRect;
				stretchRect.x = 0;
				stretchRect.y = 0;
				stretchRect.w = SCREEN_WIDTH;
				stretchRect.h = SCREEN_HEIGHT;
#ifdef _WIN32
				SDL_BlitSurfaceScaled(gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT], NULL, gScreenSurface, &stretchRect, SDL_SCALEMODE_NEAREST);
#elif __linux__
				SDL_BlitSurface(gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT], NULL, gScreenSurface, NULL);
#endif
#ifdef _WIN32
				SDL_BlitSurfaceScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect, SDL_SCALEMODE_NEAREST);
#elif __linux__
				SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
#endif
				SDL_RenderTexture(gRenderer, gTexture, NULL, NULL);
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	
	close();
	delete timer;
    delete clock;
    return 0;
}
