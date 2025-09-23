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
#include "Load.h"

#ifdef _WIN32
#elif __linux__
#endif


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;
SDL_Surface* gXOut = NULL;

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

	const char* p = load->Path("hello_world.bmp");
	printf("> %s\n", p);

	const char* basePath = SDL_GetBasePath();
#ifdef _WIN32
	const char* relativePath = "\\art\\hello_world.bmp";
#elif __linux__
	const char* relativePath = "SoftfootFalls/x64/debug/art/hello_world.bmp";
#endif
	int pathSize = strlen(basePath) + strlen(relativePath) + 1;
	char* path = new char[pathSize];
	for(int i = 0; i < pathSize; ++i)
	{
		path[i] = (i < (int)strlen(basePath)) ? basePath[i] : relativePath[i - strlen(basePath)];
	}
	gHelloWorld = SDL_LoadBMP(p);
	if (gHelloWorld == NULL)
	{
		SDL_Log("Unable to load image %s! SDL Error: %s\n", "hello_world.bmp", SDL_GetError());
		success = false;
	}
	delete[] path;
	delete load;

	return success;
}

void close()
{
#ifdef _WIN32
	SDL_DestroySurface(gHelloWorld);
#elif __linux__
	SDL_FreeSurface(gHelloWorld);
#endif
	gHelloWorld = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	SDL_Quit();
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
			SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
			SDL_UpdateWindowSurface(gWindow);
		}
	}
	SDL_UpdateWindowSurface(gWindow);
	SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);

	SDL_Event e;
	bool quit = false;
	while (quit == false)
	{
		timer->Update();
		while (SDL_PollEvent(&e))
		{
#ifdef _WIN32
			if (e.type == SDL_EVENT_QUIT)
#elif __linux__
			if (e.type == SDL_QUIT)
#endif
				quit = true;
		}
	}
	close();
	delete timer;
    delete clock;
    return 0;
}
