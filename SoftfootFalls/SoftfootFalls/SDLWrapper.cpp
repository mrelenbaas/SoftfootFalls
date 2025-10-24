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

bool LTexture::LoadFromFile(const char* path)
{
	if (!LoadPixelsFromFile(path))
		Print::WithClassName("ERROR: Failed to load pixels: ", path);
	else if (!LoadFromPixels())
		Print::WithClassName(
			"ERROR: Failed to make texture from pixels from: ",
			path);
	return mTexture != NULL;
}

bool LTexture::LoadPixelsFromFile(const char* path)
{
	Free();
	surface = IMG_Load(path);
	if (surface == NULL)
		Print::WithClassName("ERROR: Failed to load image:", path);
	else
	{
		mWidth = surface->w;
		mHeight = surface->h;
	}
	return surface != NULL;
}

bool LTexture::LoadFromPixels()
{
	if (surface == NULL)
	{
		Print::WithClassName("ERROR: No pixels loaded.", "");
	}
	else
	{
		mTexture = SDL_CreateTextureFromSurface(renderer, surface);
		if (mTexture == NULL)
		{
			SDL_Log("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			mWidth = surface->w;
			mHeight = surface->h;
		}
		surface = NULL;
	}
	return mTexture != NULL;
}
