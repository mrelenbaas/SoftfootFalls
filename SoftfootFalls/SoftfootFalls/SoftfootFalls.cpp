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

	const char* helloWorld = load->Path("hello_world.bmp");
	gHelloWorld = SDL_LoadBMP(helloWorld);
	load->Print(gHelloWorld, helloWorld);

	const char* xOut = load->Path("x.bmp");
	gXOut = SDL_LoadBMP(xOut);
	load->Print(gXOut, xOut);
	
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
#ifdef _WIN32
	SDL_DestroySurface(gXOut);
#elif __linux__
	SDL_FreeSurface(gXOut);
#endif
	gXOut = NULL;

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
			bool quit = false;
			SDL_Event e;
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
				}
				SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
				SDL_BlitSurface(gXOut, NULL, gScreenSurface, NULL);
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}
	
	close();
	delete timer;
    delete clock;
    return 0;
}
