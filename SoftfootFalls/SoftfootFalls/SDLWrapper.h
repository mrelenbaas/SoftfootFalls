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
		, mTexture(nullptr)
		, surface(nullptr)
		, mRawPixels(nullptr)
		, mRawPitch(0)
		, mWidth(0)
		, mHeight(0) {}
	~LTexture();
	void Init(SDL_Renderer* renderer);
	bool LoadFromFile(const char*);
	bool LoadPixelsFromFile(const char*);
	bool LoadFromPixels();
	void Free();
#ifdef _WIN32
	void render(int x, int y, SDL_FRect* clip = NULL, double angle = 0.0, SDL_FPoint* center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE, Distance distance = { 0, 0 });
#elif __linux__
	void render(int x, int y, SDL_Rect* clip = NULL, double secondAngle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, Distance distance = { 0, 0 });
#endif
	int GetWidth() const;
	int GetHeight() const;
	Uint32 getPixel32(Uint32 x, Uint32 y);
	Uint32 getPitch32();
	SDL_Texture* getTexture();
private:
	SDL_Renderer* renderer;
	SDL_Texture* mTexture;
	SDL_Surface* surface;
	void* mRawPixels;
	int mRawPitch;
	int mWidth;
	int mHeight;
};