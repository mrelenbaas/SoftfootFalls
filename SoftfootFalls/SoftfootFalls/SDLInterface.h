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
const SDL_Keycode KEY_Z = SDLK_Z;
#elif __linux__
const SDL_Keycode KEY_Z = SDLK_z;
#endif
#ifdef _WIN32
const SDL_Keycode KEY_C = SDLK_C;
#elif __linux__
const SDL_Keycode KEY_C = SDLK_c;
#endif

#ifdef _WIN32
const SDL_Keycode KEY_X = SDLK_X;
#elif __linux__
const SDL_Keycode KEY_X = SDLK_x;
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
//  CONTROLLER  ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32
const SDL_EventType EVENT_JOYSTICK_BUTTON_DOWN = SDL_EVENT_JOYSTICK_BUTTON_DOWN;
#elif __linux__
const SDL_EventType EVENT_JOYSTICK_BUTTON_DOWN = SDL_JOYBUTTONDOWN;
#endif

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
//  SURFACE  //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void DestroySurface(SDL_Surface*);

///////////////////////////////////////////////////////////////////////
//  RENDER  ///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

SDL_Renderer* SetLinuxRenderer(SDL_Window*);

void RenderLine(SDL_Renderer*, float, float, float, float);

#ifdef _WIN32
void RenderTexture(SDL_Renderer*, SDL_Texture*, double angle, SDL_FlipMode flipMode);
#elif __linux__
void RenderTexture(SDL_Renderer*, SDL_Texture*, double angle, SDL_RendererFlip flipMode);
#endif

void SetRenderViewport(SDL_Renderer*, SDL_Rect*);

///////////////////////////////////////////////////////////////////////
//  FILE I/O  /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32
SDL_IOStream* GetFile();
#elif __linux__
SDL_RWops* GetFile();
#endif

#ifdef _WIN32
SDL_IOStream* IOFromFile(const char*, const char*);
#elif __linux__
SDL_RWops* IOFromFile(const char*, const char*);
#endif

#ifdef _WIN32
void ReadIO(SDL_IOStream*, void*);
#elif __linux__
void ReadIO(SDL_RWops*, void*);
#endif

#ifdef _WIN32
void WriteIO(SDL_IOStream*, const void*);
#elif __linux__
void WriteIO(SDL_RWops*, const void*);
#endif

#ifdef _WIN32
void CloseIO(SDL_IOStream*);
#elif __linux__
void CloseIO(SDL_RWops*);
#endif

///////////////////////////////////////////////////////////////////////
//  SFX  //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void PlaySFX(const char*);
