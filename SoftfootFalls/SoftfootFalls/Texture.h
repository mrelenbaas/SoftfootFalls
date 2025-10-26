#include <iostream>

#ifdef _WIN32
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <windows.h>
#include <mmsystem.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdlib>
#endif


struct Distance
{
	float width;
	float height;
};

class Texture
{
public:
	Texture()
		: renderer(nullptr)
		, texture(nullptr)
		, surface(nullptr)
		, rawPixels(nullptr)
		, rawPitch(0)
		, width(0)
		, height(0) {
	}
	~Texture();
	bool Init(SDL_Renderer* renderer, const char*);
	void Free();
	bool LoadFromFile(const char*);
	bool LoadPixelsFromFile(const char*);
	bool LoadFromPixels();
	void Draw(SDL_Rect*, double angle = 0.0);
	int GetWidth() const { return width; };
	int GetHeight() const { return height; };
	Uint32 GetPixel32(Uint32, Uint32);
	Uint32 GetPitch32() { return surface->pitch / 4; };
	SDL_Texture* GetTexture() { return texture; };
private:
	void SetWidthAndHeight()
	{
		width = surface->w;
		height = surface->h;
	};
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Surface* surface;
	void* rawPixels;
	int rawPitch;
	int width;
	int height;
};