#include "SDLInterface.h"


///////////////////////////////////////////////////////////////////////
//  KEYS  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32
SDL_Keycode Key(SDL_Event event)
{
	return event.key.key;
}
#elif __linux__
SDL_Keycode Key(SDL_Event event)
{
	return event.key.keysym.sym;
}
#endif

///////////////////////////////////////////////////////////////////////
//  WINDOW  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool FirstInit()
{
	bool result = NULL;

#ifdef _WIN32
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO))
	{
		result = true;
	}
#elif __linux__
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
	{
		result = false;
	}
#endif

	return result;
}

void SecondInit(const char* title, int width, int height, SDL_Window** window, SDL_Renderer** renderer)
{
#ifdef _WIN32
	SDL_CreateWindowAndRenderer(title, width, height, SDL_WINDOW_RESIZABLE, window, renderer);
	SDL_SetRenderVSync(*renderer, 1);
#elif __linux__
	*window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
}

bool IsWindowQuit(SDL_Event event)
{
#ifdef _WIN32
	return event.type == SDL_EVENT_QUIT;
#elif __linux__
	return event.type == SDL_QUIT;
#endif
}

bool IsResized(SDL_Event& event)
{
	bool result = false;
#ifdef _WIN32
	if (event.type == SDL_EVENT_WINDOW_RESIZED)
#elif __linux__
	if ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED))
#endif
	{
		result = true;
	}
	return result;
}

///////////////////////////////////////////////////////////////////////
//  INPUT  ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void StartTextInput(SDL_Window* window)
{
#ifdef _WIN32
	SDL_StartTextInput(window);
#elif __linux__
	SDL_StartTextInput();
#endif
}

void StopTextInput(SDL_Window* window)
{
#ifdef _WIN32
	SDL_StopTextInput(window);
#elif __linux__
	SDL_StopTextInput();
#endif
}

///////////////////////////////////////////////////////////////////////
//  SURFACE  //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void DestroySurface(SDL_Surface* surface)
{
#ifdef _WIN32
	SDL_DestroySurface(surface);
#elif __linux__
	SDL_FreeSurface(surface);
#endif
}

///////////////////////////////////////////////////////////////////////
//  RENDER  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void RenderLine(SDL_Renderer* renderer, float x, float y, float w, float h)
{
#if _WIN32
	SDL_RenderLine(renderer, x, y, w, h);
#elif __linux__
	SDL_RenderDrawLine(renderer, x, y, w, h);
#endif
}

void RenderTexture(SDL_Renderer* renderer, SDL_Texture* texture)
{
#ifdef _WIN32
	SDL_RenderTexture(renderer, texture, NULL, NULL);
#elif __linux__
	SDL_RenderCopy(renderer, texture, NULL, NULL);
#endif
}

void SetRenderViewport(SDL_Renderer* renderer, SDL_Rect* rect)
{
#if _WIN32
	SDL_SetRenderViewport(renderer, rect);
#elif __linux__
	SDL_RenderSetViewport(renderer, rect);
#endif
}
