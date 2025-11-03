#include "Texture.h"
#include "SDLInterface.h"


Texture::~Texture()
{
	Free();
}

bool Texture::Init(SDL_Renderer* renderer, const char* name)
{
	(*this).renderer = renderer;
	return LoadFromFile(name);
}

void Texture::Free()
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

bool Texture::LoadFromFile(const char* path)
{
	if (!LoadPixelsFromFile(path)) {}
	else LoadFromPixels();
	return texture != NULL;
}

bool Texture::LoadPixelsFromFile(const char* path)
{
	Free();
	surface = IMG_Load(path);
	SetWidthAndHeight();
	return surface != NULL;
}

bool Texture::LoadFromPixels()
{
	if (surface == NULL) {}
	else
	{
		texture = SDL_CreateTextureFromSurface(renderer, surface);
		SetWidthAndHeight();
		DestroySurface(surface);
		surface = NULL;
	}
	return texture != NULL;
}

#ifdef _WIN32
void Texture::Draw(SDL_Rect* viewport, double angle, SDL_FlipMode flipMode)
#elif __linux__
void Texture::Draw(SDL_Rect* viewport, double angle, SDL_RendererFlip flipMode)
#endif
{
	SetRenderViewport(renderer, viewport);
	RenderTexture(renderer, texture, angle, flipMode);
}

Uint32 Texture::GetPixel32(Uint32 x, Uint32 y)
{
	return static_cast<Uint32*>(surface->pixels)[(y * GetPitch32()) + x];
}
