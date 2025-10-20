#include "SDLInterface.h"


///////////////////////////////////////////////////////////////////////
//  WINDOW  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool IsWindowQuit(SDL_Event e)
{
#ifdef _WIN32
	return e.type == SDL_EVENT_QUIT;
#elif __linux__
	return e.type == SDL_QUIT;
#endif
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
