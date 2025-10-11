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


///////////////////////////////////////////////////////////////////////
//  KEYS  /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32
	const SDL_Keycode KEY_W = SDLK_W;
#elif __linux__
	const SDL_Keycode KEY_W = SDLK_w;
#endif
#ifdef _WIN32
	const SDL_Keycode KEY_A = SDLK_A;
#elif __linux__
	const SDL_Keycode KEY_A = SDLK_a;
#endif
#ifdef _WIN32
	const SDL_Keycode KEY_S = SDLK_S;
#elif __linux__
	const SDL_Keycode KEY_S = SDLK_s;
#endif
#ifdef _WIN32
	const SDL_Keycode KEY_D = SDLK_D;
#elif __linux__
	const SDL_Keycode KEY_D = SDLK_d;
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

#if _WIN32
void RenderRect(SDL_Renderer*, SDL_FRect* rect);
#elif __linux__
void RenderRect(SDL_Renderer*, SDL_Rect* rect);
#endif

void RenderTexture(SDL_Renderer*, SDL_Texture*);

void SetRenderViewport(SDL_Renderer*, SDL_Rect*);
