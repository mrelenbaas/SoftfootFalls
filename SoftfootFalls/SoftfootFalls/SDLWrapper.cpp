#include "SDLWrapper.h"
#include "SDLInterface.h"

#include "Print.h"


LTexture::~LTexture()
{
	Free();
}

void LTexture::Init(SDL_Renderer* renderer)
{
	(*this).renderer = renderer;
}

void LTexture::Free()
{
	if (texture)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
	if (surface)
	{
		DestroySurface(surface);
		surface = NULL;
	}
}

bool LTexture::LoadFromFile(const char* path)
{
	if (!LoadPixelsFromFile(path))
		Print::WithClassName("ERROR: Failed to load pixels: ", path);
	else if (!LoadFromPixels())
		Print::WithClassName(
			"ERROR: Failed to make texture from pixels from: ",
			path);
	return texture != NULL;
}

bool LTexture::LoadPixelsFromFile(const char* path)
{
	Free();
	surface = IMG_Load(path);
	if (surface == NULL)
		Print::WithClassName("ERROR: Failed to load image:", path);
	else SetWidthAndHeight();
	return surface != NULL;
}

bool LTexture::LoadFromPixels()
{
	if (surface == NULL)
		Print::WithClassName("ERROR: No pixels loaded.", SDL_GetError());
	else
	{
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == NULL)
			Print::WithClassName(
				"ERROR: Failed to create texture from loaded pixels:",
				SDL_GetError());
		else SetWidthAndHeight();
		DestroySurface(surface);
		surface = NULL;
	}
	return texture != NULL;
}

Uint32 LTexture::GetPixel32(Uint32 x, Uint32 y)
{
	return static_cast<Uint32*>(surface->pixels)[(y * GetPitch32()) + x];
}
