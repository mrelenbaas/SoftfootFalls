#include <stdio.h>
#include <vector>

#include "SDLInterface.h"

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#elif __linux__
#include <cstdlib>
#endif
#include <string>



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
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
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

SDL_Renderer* GetLinuxRenderer(SDL_Window* window)
{
	SDL_Renderer* renderer = NULL;
#ifdef __linux__
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return NULL;
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			return NULL;
		}
	}
#endif
	return renderer;
}

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

///////////////////////////////////////////////////////////////////////
//  FILE I/O  /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32
SDL_IOStream* GetFile()
#elif __linux__
SDL_RWops* GetFile()
#endif
{
#ifdef _WIN32
	SDL_IOStream* file = NULL;
#elif __linux__
	SDL_RWops* file = NULL;
#endif
	return file;
}

#ifdef _WIN32
SDL_IOStream* IOFromFile(const char* file, const char* mode)
#elif __linux__
SDL_RWops* IOFromFile(const char* file, const char* mode)
#endif
{
#ifdef _WIN32
	return SDL_IOFromFile(file, mode);
#elif __linux__
	return SDL_RWFromFile(file, mode);
#endif
}

#ifdef _WIN32
void ReadIO(SDL_IOStream* file, void* data)
#elif __linux__
void ReadIO(SDL_RWops* file, void* data)
#endif
{
#ifdef _WIN32
	SDL_ReadIO(file, data, sizeof(Sint32));
#elif __linux__
	SDL_RWread(file, data, sizeof(Sint32), 1);
#endif
}

#ifdef _WIN32
void WriteIO(SDL_IOStream* file, const void* data)
#elif __linux__
void WriteIO(SDL_RWops* file, const void* data)
#endif
{
#ifdef _WIN32
	SDL_WriteIO(file, data, sizeof(Sint32));
#elif __linux__
	SDL_RWwrite(file, data, sizeof(Sint32), 1);
#endif
}

#ifdef _WIN32
void CloseIO(SDL_IOStream* file)
#elif __linux__
void CloseIO(SDL_RWops* file)
#endif
{
#ifdef _WIN32
	SDL_CloseIO(file);
#elif __linux__
	SDL_RWclose(file);
#endif
}

///////////////////////////////////////////////////////////////////////
//  SFX  //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

void PlaySFX(const char* path)
{
#ifdef _WIN32
	size_t requiredSize;
	mbstowcs_s(&requiredSize, nullptr, 0, path, _TRUNCATE);
	std::vector<wchar_t> wideBuffer(requiredSize);
	mbstowcs_s(&requiredSize, wideBuffer.data(), requiredSize, path, _TRUNCATE);
	std::wstring wideString(wideBuffer.data());
	PlaySound(wideString.c_str(), NULL, SND_FILENAME | SND_ASYNC);
#elif __linux__
	system("aplay ~/SoftfootFalls/SoftfootFalls/x64/Debug/art/scratch.wav");
#endif
}
