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

///////////////////////////////////////////////////////////////////////
//  KEYS  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef WINDOWS
	const SDL_Keycode KEY_W = SDLK_W;
#elif LINUX
	const SDL_Keycode KEY_W = case SDLK_w;
#endif
#ifdef WINDOWS
	const SDL_Keycode KEY_A = SDLK_A;
#elif LINUX
	const SDL_Keycode KEY_A = case SDLK_a;
#endif
#ifdef WINDOWS
	const SDL_Keycode KEY_S = SDLK_S;
#elif LINUX
	const SDL_Keycode KEY_S = case SDLK_s;
#endif
#ifdef WINDOWS
	const SDL_Keycode KEY_D = SDLK_D;
#elif LINUX
	const SDL_Keycode KEY_D = case SDLK_d;
#endif

///////////////////////////////////////////////////////////////////////
//  WINDOW  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

bool IsWindowQuit(SDL_Event);

///////////////////////////////////////////////////////////////////////
//  RENDER  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void RenderLine(SDL_Renderer*, float, float, float, float);

void RenderPoint(SDL_Renderer*, float, float);

#if WINDOWS
void RenderRect(SDL_Renderer*, SDL_FRect* rect);
#elif LINUX
void RenderRect(SDL_Renderer*, SDL_Rect* rect);
#endif
