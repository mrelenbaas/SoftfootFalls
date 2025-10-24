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

class LTexture
{
public:
	LTexture()
		: renderer(nullptr)
		, texture(nullptr)
		, surface(nullptr)
		, rawPixels(nullptr)
		, rawPitch(0)
		, mWidth(0)
		, mHeight(0) {}
	~LTexture();
	void Init(SDL_Renderer* renderer);
	void Free();
	bool LoadFromFile(const char*);
	bool LoadPixelsFromFile(const char*);
	bool LoadFromPixels();
#ifdef _WIN32
	void render(int x, int y, SDL_FRect* clip = NULL, double angle = 0.0, SDL_FPoint* center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE, Distance distance = { 0, 0 });
#elif __linux__
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, Distance distance = { 0, 0 });
#endif
	int GetWidth() const { return mWidth; };
	int GetHeight() const { return mHeight; };
	Uint32 GetPixel32(Uint32, Uint32);
	Uint32 GetPitch32() { return surface->pitch / 4; };
	SDL_Texture* getTexture() { return texture; };
private:
	void SetWidthAndHeight()
	{
		mWidth = surface->w;
		mHeight = surface->h;
	};
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Surface* surface;
	void* rawPixels;
	int rawPitch;
	int mWidth;
	int mHeight;
};