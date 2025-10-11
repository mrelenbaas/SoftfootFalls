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

void RenderPoint(SDL_Renderer* renderer, float x, float y)
{
#ifdef _WIN32
	SDL_RenderPoint(renderer, x, y);
#elif __linux__
	SDL_RenderDrawPoint(renderer, x, y);
#endif
}

#if _WIN32
void RenderRect(SDL_Renderer* renderer, SDL_FRect* rect)
#elif __linux__
void RenderRect(SDL_Renderer* renderer, SDL_Rect* rect)
#endif
{
#if _WIN32
	SDL_RenderRect(renderer, rect);
#elif __linux__
	SDL_RenderDrawRect(renderer, rect);
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
