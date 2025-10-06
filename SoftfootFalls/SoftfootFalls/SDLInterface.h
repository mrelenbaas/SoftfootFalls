#ifdef _WIN32
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_mixer.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#endif

#define WINDOWS SDL_MAJOR_VERSION >= 3 & _WIN32
#define LINUX SDL_MAJOR_VERSION < 3 & __linux__


#if _WIN32
#elif __linux__
#endif

constexpr SDL_Keycode KeyHome()
{
	return SDLK_HOME;
}

constexpr SDL_Keycode KeyEnd()
{
	return SDLK_END;
}

constexpr SDL_Keycode KeyP()
{
#if WINDOWS
	return SDLK_P;
#elif LINUX
	return SDLK_p;
#endif
}

void RenderLine(SDL_Renderer*, float, float, float, float);

void RenderPoint(SDL_Renderer*, float, float);

#if _WIN32
void RenderRect(SDL_Renderer*, SDL_FRect* rect);
#elif __linux__
void RenderRect(SDL_Renderer*, SDL_Rect* rect);
#endif
