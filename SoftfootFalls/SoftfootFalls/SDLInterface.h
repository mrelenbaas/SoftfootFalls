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

#ifdef _WIN32
		const SDL_EventType KEY_RELEASED = SDL_EVENT_KEY_UP;
#elif __linux__
		const SDL_EventType KEY_RELEASED = SDL_KEYUP;
#endif
#ifdef _WIN32
		const SDL_EventType KEY_PRESSED = SDL_EVENT_KEY_DOWN;
#elif __linux__
		const SDL_EventType KEY_PRESSED = SDL_KEYDOWN;
#endif

SDL_Keycode Key(SDL_Event event);

///////////////////////////////////////////////////////////////////////
//  WINDOW  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32
const SDL_EventType EVENT_WINDOW_MINIMIZED = SDL_EVENT_WINDOW_MINIMIZED;
#elif __linux__
const SDL_WindowEventID EVENT_WINDOW_MINIMIZED = SDL_WINDOWEVENT_MINIMIZED;
#endif
#ifdef _WIN32
const SDL_EventType EVENT_WINDOW_MAXIMIZED = SDL_EVENT_WINDOW_MAXIMIZED;
#elif __linux__
const SDL_WindowEventID EVENT_WINDOW_MAXIMIZED = SDL_WINDOWEVENT_MAXIMIZED;
#endif
#ifdef _WIN32
const SDL_EventType EVENT_WINDOW_RESTORED = SDL_EVENT_WINDOW_RESTORED;
#elif __linux__
const SDL_WindowEventID EVENT_WINDOW_RESTORED = SDL_WINDOWEVENT_RESTORED;
#endif

bool Terrtronics_Init();

bool IsWindowQuit(SDL_Event);

///////////////////////////////////////////////////////////////////////
//  INPUT  ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void StartTextInput(SDL_Window*);

void StopTextInput(SDL_Window*);

///////////////////////////////////////////////////////////////////////
//  SURFACE  //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void DestroySurface(SDL_Surface*);

///////////////////////////////////////////////////////////////////////
//  RENDER  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void RenderLine(SDL_Renderer*, float, float, float, float);

void RenderTexture(SDL_Renderer*, SDL_Texture*);

void SetRenderViewport(SDL_Renderer*, SDL_Rect*);
