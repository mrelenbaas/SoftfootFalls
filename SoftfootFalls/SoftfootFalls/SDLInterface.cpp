#include "SDLInterface.h"
#include "Container.h"


///////////////////////////////////////////////////////////////////////
//  KEYS  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

SDL_Keycode Key(SDL_Event event)
{
#ifdef _WIN32
	return event.key.key;
#elif __linux__
	return event.key.keysym.sym;
#endif
}

///////////////////////////////////////////////////////////////////////
//  CONTROLLER  ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//  WINDOW  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool FirstInit()
{
	bool result = NULL;

#ifdef _WIN32
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_RENDERER_VSYNC_ADAPTIVE | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO))
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
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
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
	SetRenderViewport(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
#if _WIN32
	SDL_RenderLine(renderer, x, y, w, h);
#elif __linux__
	SDL_RenderDrawLine(renderer, x, y, w, h);
#endif
}

#ifdef _WIN32
void RenderTexture(SDL_Renderer* renderer, SDL_Texture* texture, double angle, SDL_FlipMode flipMode)
#elif __linux__
void RenderTexture(SDL_Renderer* renderer, SDL_Texture* texture, double angle, SDL_RendererFlip flipMode)
#endif
{
#ifdef _WIN32
	SDL_RenderTextureRotated(renderer, texture, NULL, NULL, angle, NULL, flipMode);
#elif __linux__
	SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, flipMode);
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

#ifdef _WIN32
void RenderTextureRotated(SDL_Renderer* renderer, SDL_Texture* texture, SDL_FRect* clip, SDL_FRect* renderQuad, double angle, SDL_FPoint* center, SDL_FlipMode flip)
#elif __linux__
void RenderTextureRotated(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect* clip, SDL_Rect* renderQuad, double angle, SDL_Point* center, SDL_RendererFlip flip)
#endif
{
#ifdef _WIN32
	SDL_RenderTextureRotated(renderer, texture, clip, renderQuad, angle, center, flip);
#elif __linux__
	SDL_RenderCopyEx(renderer, texture, clip, renderQuad, angle, center, flip);
#endif
}
