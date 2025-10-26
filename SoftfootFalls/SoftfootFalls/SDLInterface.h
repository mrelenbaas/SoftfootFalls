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
//  SHAPES  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

//#ifdef _WIN32
//SDL_FRect Rect(int, int, int, int);
//#elif __linux__
//SDL_Rect Rect(int, int, int, int);
//#endif
SDL_Rect Rect(int, int, int, int);

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
	const SDL_Keycode KEY_Q = SDLK_Q;
#elif __linux__
	const SDL_Keycode KEY_Q = SDLK_q;
#endif
#ifdef _WIN32
	const SDL_Keycode KEY_E = SDLK_E;
#elif __linux__
	const SDL_Keycode KEY_E = SDLK_e;
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

bool FirstInit();

void SecondInit(const char*, int, int, SDL_Window**, SDL_Renderer**);

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

bool IsWindowQuit(SDL_Event);

bool IsResized(SDL_Event&);

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

void RenderTexture(SDL_Renderer*, SDL_Texture*, double angle = 0.0);

void SetRenderViewport(SDL_Renderer*, SDL_Rect*);

//#ifdef _WIN32
//	void RenderTextureRotated(SDL_Renderer*, SDL_Texture*, SDL_FRect*, SDL_FRect*, double, SDL_FPoint*, SDL_FlipMode);
//#elif __linux__
//	void RenderTextureRotated(SDL_Renderer*, SDL_Texture*, SDL_Rect*, SDL_Rect*, double, SDL_Point*, SDL_RendererFlip);
//#endif
