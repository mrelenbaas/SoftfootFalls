#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <functional>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <fstream>

#include "Print.h"
#include "Time.h"
#include "Clock.h"
#include "Timer.h"
#include "SoftfootFalls.h"

#ifdef _WIN32
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_mixer.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#endif
#include "SDLInterface.h"
#include "Load.h"


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

const int SCREEN_FPS = 60;
const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

const int JOYSTICK_DEAD_ZONE = 8000;
#ifdef _WIN32
const double M_PI = 3.14159265359;
#endif

const int TOTAL_DATA = 10;

const int TOTAL_PARTICLES = 20;

const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;

enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
	BUTTON_SPRITE_MOUSE_DOWN = 2,
	BUTTON_SPRITE_MOUSE_UP = 3,
	BUTTON_SPRITE_TOTAL = 4
};

enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

enum Directions
{
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
	DIRECTION_TOTAL
};

class LTexture
{
public:
	LTexture();
	~LTexture();
	bool loadFromFile(const char* path);
	bool loadPixelsFromFile(const char* path);
	bool loadFromPixels();
	bool createBlank(int width, int height, SDL_TextureAccess access);
	void free();
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);
#ifdef _WIN32
	void render(int x, int y, SDL_FRect* clip = NULL, double angle = 0.0, SDL_FPoint* center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE);
#elif __linux__
	void render(int x, int y, SDL_Rect* clip = NULL, double secondAngle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
#endif
	void setAsRenderTarget();
	int getWidth();
	int getHeight();
	Uint32* getPixels32();
	Uint32 getPixel32(Uint32 x, Uint32 y);
	Uint32 getPitch32();
	Uint32 mapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	void copyRawPixels32(void* pixels);
	bool lockTexture();
	bool unlockTexture();
	SDL_Texture* getTexture();
private:
	SDL_Texture* mTexture;
	SDL_Surface* mSurfacePixels;
	void* mRawPixels;
	int mRawPitch;
	int mWidth;
	int mHeight;
};
SDL_Texture* LTexture::getTexture()
{
	return mTexture;
}

class LButton
{
public:
	LButton();
	void setPosition(int x, int y);
	void handleEvent(SDL_Event* e);
	void render();
private:
	SDL_Point mPosition;
	LButtonSprite mCurrentSprite;
};

class LTimer
{
public:
	LTimer();
	void start();
	void stop();
	void pause();
	void unpause();
	Uint64 getTicks();
	bool isStarted();
	bool isPaused();
private:
	Uint64 mStartTicks;
	Uint64 mPausedTicks;
	bool mPaused;
	bool mStarted;
};

class Particle
{
public:
	Particle(int x, int y);
	void render();
	bool isDead();
private:
	int mPosX, mPosY;
	int mFrame;
	LTexture* mTexture;
};

class Dot
{
public:
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	static const int DOT_VEL = 640;
	Dot();
	~Dot();
	void handleEvent(SDL_Event& e);
	void move(float timestep);
	void setCamera(SDL_Rect& camera);
	void render(SDL_Rect& camera);
private:
	Particle* particles[TOTAL_PARTICLES];
	void renderParticles();
	SDL_Rect mBox;
	int mVelX, mVelY;
};

class LWindow
{
public:
	LWindow();
	bool init(bool bCreateRenderer = true);
#ifdef __linux__
	SDL_Renderer* createRenderer();
#endif
	SDL_Renderer* getRenderer();
	void handleEvent(SDL_Event& e);
	void free();
	int getWidth();
	int getHeight();
	SDL_Window* getWindow();
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();
private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	int mWidth;
	int mHeight;
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
};

class LBitmapFont
{
public:
	LBitmapFont();
	bool buildFont(std::string path);
	void free();
	void renderText(int x, int y, std::string text);
private:
	LTexture mFontTexture;
#ifdef _WIN32
	SDL_FRect mChars[256];
#elif __linux__
	SDL_Rect mChars[256];
#endif
	int mNewLine, mSpace;
};

class DataStream
{
public:
	DataStream();
	bool loadMedia();
	void free();
	void* getBuffer();
private:
	SDL_Surface* mImages[4];
	int mCurrentImage;
	int mDelayFrames;
};

bool init();
bool loadMedia();
void close();
SDL_Surface* loadSurface(const char* path, Load* load, bool* success);
SDL_Texture* loadTexture(const char* path, Load* load, bool* success);

LWindow gWindow;
SDL_Renderer* gRenderer = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Texture* gTexture = NULL;
LTexture gModulatedTexture;
const int WALKING_ANIMATION_FRAMES = 4;
#ifdef _WIN32
SDL_FRect gWalkingSpriteClips[WALKING_ANIMATION_FRAMES];
#elif __linux__
SDL_Rect gWalkingSpriteClips[WALKING_ANIMATION_FRAMES];
#endif
LTexture gWalkingSpriteSheetTexture;
LTexture gIconCursor;
LTexture gSun;
LTexture gMoon;
#ifdef _WIN32
SDL_FRect gSpriteClips[BUTTON_SPRITE_TOTAL];
#elif __linux__
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
#endif
LTexture gButtonSpriteSheetTexture;
LButton gButtons[TOTAL_BUTTONS];
#ifdef _WIN32
SDL_Gamepad* gGameController;
#elif __linux__
SDL_GameController* gGameController;
#endif
SDL_Joystick* gJoystick = NULL;
SDL_Haptic* gJoyHaptic = NULL;
//MIX_Mixer* gMusic = NULL;
//MIX_Audio* gBeat = NULL;
//MIX_Audio* gScratch = NULL;
//MIX_Audio* gHigh = NULL;
//MIX_Audio* gMedium = NULL;
//MIX_Audio* gLow = NULL;
//#ifdef _WIN32
static SDL_AudioStream* stream = NULL;
//#elif __linux__
//static SDL_AudioFormat stream;
//#endif
static int current_sine_sample = 0;
LTexture gDotTexture;
LTexture gBGTexture;
const int HORIZON_SIZE = 8;
LTexture gHorizons[HORIZON_SIZE];
LTexture gBoxes[KEY_PRESS_SURFACE_TOTAL];
LTexture* gBox = &gBoxes[KEY_PRESS_SURFACE_UP];
Sint32 gData[TOTAL_DATA];
LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;
LBitmapFont gBitmapFont;
LTexture gTargetTexture;
Directions gDirection = DIRECTION_UP;


LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mSurfacePixels = NULL;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(const char* path)
{
	if (!loadPixelsFromFile(path))
	{
		SDL_Log("Failed to load pixels for %s!\n", path);
	}
	else
	{
		if (!loadFromPixels())
		{
			SDL_Log("Failed to texture from pixels from %s!\n", path);
		}
	}
	return mTexture != NULL;
}

bool LTexture::loadPixelsFromFile(const char* path)
{
	free();
	mSurfacePixels = IMG_Load(path);
	if (mSurfacePixels == NULL)
	{
		SDL_Log("Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
	}
	else
	{
		mWidth = mSurfacePixels->w;
		mHeight = mSurfacePixels->h;
	}
	return mSurfacePixels != NULL;
}

bool LTexture::loadFromPixels()
{
	if (mSurfacePixels == NULL)
	{
		SDL_Log("No pixels loaded!");
	}
	else
	{
#ifdef _WIN32
		SDL_SetSurfaceColorKey(mSurfacePixels, true, SDL_MapSurfaceRGB(mSurfacePixels, 0, 0xFF, 0xFF));
#elif __linux__
		SDL_SetColorKey(mSurfacePixels, SDL_TRUE, SDL_MapRGB(mSurfacePixels->format, 0, 0xFF, 0xFF));
#endif
		mTexture = SDL_CreateTextureFromSurface(gRenderer, mSurfacePixels);
		if (mTexture == NULL)
		{
			SDL_Log("Unable to create texture from loaded pixels! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			mWidth = mSurfacePixels->w;
			mHeight = mSurfacePixels->h;
		}
#ifdef _WIN32
		SDL_DestroySurface(mSurfacePixels);
#elif __linux__
		SDL_FreeSurface(mSurfacePixels);
#endif
		mSurfacePixels = NULL;
	}
	return mTexture != NULL;
}

bool LTexture::createBlank(int width, int height, SDL_TextureAccess access)
{
	free();
	mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
	if (mTexture == NULL)
	{
		SDL_Log("Unable to create streamable blank texture! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		mWidth = width;
		mHeight = height;
#ifdef __linux__
		SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderTarget(gRenderer, mTexture);
		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(gRenderer);
		SDL_SetRenderTarget(gRenderer, NULL);
#endif
	}
	return mTexture != NULL;
}

void LTexture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
	if (mSurfacePixels != NULL)
	{
#ifdef _WIN32
		SDL_DestroySurface(mSurfacePixels);
#elif __linux__
		SDL_FreeSurface(mSurfacePixels);
#endif
		mSurfacePixels = NULL;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

#ifdef _WIN32
void LTexture::render(int x, int y, SDL_FRect* clip, double angle, SDL_FPoint* center, SDL_FlipMode flip)
#elif __linux__
void LTexture::render(int x, int y, SDL_Rect* clip, double secondAngle, SDL_Point* center, SDL_RendererFlip flip)
#endif
{
#ifdef _WIN32
	SDL_FRect renderQuad = { (float)x, (float)y, (float)mWidth, (float)mHeight };
#elif __linux__
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
#endif
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}
#ifdef _WIN32
	SDL_RenderTextureRotated(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
#elif __linux__
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, secondAngle, center, flip);
#endif
}

void LTexture::setAsRenderTarget()
{
	SDL_SetRenderTarget(gRenderer, mTexture);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Uint32* LTexture::getPixels32()
{
	Uint32* pixels = NULL;
	if (mSurfacePixels != NULL)
	{
		pixels = static_cast<Uint32*>(mSurfacePixels->pixels);
	}
	return pixels;
}

Uint32 LTexture::getPixel32(Uint32 x, Uint32 y)
{
	Uint32* pixels = static_cast<Uint32*>(mSurfacePixels->pixels);
	return pixels[(y * getPitch32()) + x];
}

Uint32 LTexture::getPitch32()
{
	Uint32 pitch = 0;
	if (mSurfacePixels != NULL)
	{
		pitch = mSurfacePixels->pitch / 4;
	}
	return pitch;
}

Uint32 LTexture::mapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	Uint32 pixel = 0;
	if (mSurfacePixels != NULL)
	{
#ifdef _WIN32
		pixel = SDL_MapSurfaceRGBA(mSurfacePixels, r, g, b, a);
#elif __linux__
		pixel = SDL_MapRGBA(mSurfacePixels->format, r, g, b, a);
#endif
	}
	return pixel;
}

bool LTexture::lockTexture()
{
	bool success = true;
	if (mRawPixels != NULL)
	{
		SDL_Log("Texture is already locked!\n");
		success = false;
	}
	else
	{
#ifdef _WIN32
		if (!SDL_LockTexture(mTexture, NULL, &mRawPixels, &mRawPitch))
#elif __linux__
		if (SDL_LockTexture(mTexture, NULL, &mRawPixels, &mRawPitch) != 0)
#endif
		{
			SDL_Log("Unable to lock texture! %s\n", SDL_GetError());
			success = false;
		}
	}
	return success;
}

bool LTexture::unlockTexture()
{
	bool success = true;
	if (mRawPixels == NULL)
	{
		SDL_Log("Texture is not locked!\n");
		success = false;
	}
	else
	{
		SDL_UnlockTexture(mTexture);
		mRawPixels = NULL;
		mRawPitch = 0;
	}
	return success;
}

void LTexture::copyRawPixels32(void* pixels)
{
	if (mRawPixels != NULL)
	{
		memcpy(mRawPixels, pixels, mRawPitch * mHeight);
	}
}

LButton::LButton()
{
	mPosition.x = 0;
	mPosition.y = 0;
	mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
	mPosition.x = x;
	mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e)
{
#ifdef _WIN32
	if (e->type == SDL_EVENT_MOUSE_MOTION || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_UP)
#elif __linix__
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
#endif
	{
#ifdef _WIN32
		float x, y;
#elif __linux__
		int x, y;
#endif
		SDL_GetMouseState(&x, &y);
		bool inside = true;
		if (x < mPosition.x)
		{
			inside = false;
		}
		else if (x > mPosition.x + BUTTON_WIDTH)
		{
			inside = false;
		}
		else if (y < mPosition.y)
		{
			inside = false;
		}
		else if (y > mPosition.y + BUTTON_HEIGHT)
		{
			inside = false;
		}
		if (!inside)
		{
			mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		else
		{
			switch (e->type)
			{
#ifdef _WIN32
			case SDL_EVENT_MOUSE_MOTION:
#elif __linux__
			case SDL_MOUSEMOTION:
#endif
				mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;
#ifdef _WIN32
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
#elif __linux__
			case SDL_MOUSEBUTTONDOWN:
#endif
				mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
				break;
#ifdef _WIN32
			case SDL_EVENT_MOUSE_BUTTON_UP:
#elif __linux__
			case SDL_MOUSEBUTTONUP:
#endif
				mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			}
		}
	}
}

void LButton::render()
{
	gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}

Particle::Particle(int x, int y)
{
	mPosX = x - 5 + (rand() % 25);
	mPosY = y - 5 + (rand() % 25);
	mFrame = rand() % 5;
	switch (rand() % 3)
	{
	case 0: mTexture = &gRedTexture; break;
	case 1: mTexture = &gGreenTexture; break;
	case 2: mTexture = &gBlueTexture; break;
	}
}

void Particle::render()
{
	mTexture->render(mPosX, mPosY);
	if (mFrame % 2 == 0)
	{
		gShimmerTexture.render(mPosX, mPosY);
	}
	mFrame++;
}

bool Particle::isDead()
{
	return mFrame > 10;
}

Dot::Dot()
{
	mBox.x = 0;
	mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;
	mVelX = 0;
	mVelY = 0;
	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		particles[i] = new Particle(mBox.x, mBox.y);
	}
}

Dot::~Dot()
{
	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		delete particles[i];
	}
}

void Dot::handleEvent(SDL_Event& e)
{
#ifdef _WIN32
	if (e.type == SDL_EVENT_KEY_DOWN && e.key.repeat == 0)
#elif __linux__
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
#endif
	{
#ifdef _WIN32
		switch (e.key.key)
#elif __linux__
		switch (e.key.keysym.sym)
#endif
		{
		case SDLK_UP: mVelY -= DOT_VEL; break;
		case SDLK_DOWN: mVelY += DOT_VEL; break;
		case SDLK_LEFT: mVelX -= DOT_VEL; break;
		case SDLK_RIGHT: mVelX += DOT_VEL; break;
		}
	}
#ifdef _WIN32
	else if (e.type == SDL_EVENT_KEY_UP && e.key.repeat == 0)
#elif __linux__
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
#endif
	{
#ifdef _WIN32
		switch (e.key.key)
#elif __linux__
		switch (e.key.keysym.sym)
#endif
		{
		case SDLK_UP: mVelY += DOT_VEL; break;
		case SDLK_DOWN: mVelY -= DOT_VEL; break;
		case SDLK_LEFT: mVelX += DOT_VEL; break;
		case SDLK_RIGHT: mVelX -= DOT_VEL; break;
		}
	}
}

void Dot::move(float timeStep)
{
	mBox.x += mVelX * timeStep;
	if ((mBox.x < 0) || (mBox.x + DOT_WIDTH > LEVEL_WIDTH))
	{
		mBox.x -= mVelX;
	}
	mBox.y += mVelY * timeStep;
	if ((mBox.y < 0) || (mBox.y + DOT_HEIGHT > LEVEL_HEIGHT))
	{
		mBox.y -= mVelY;
	}
}

void Dot::setCamera(SDL_Rect& camera)
{
	camera.x = (mBox.x + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (mBox.y + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}

void Dot::render(SDL_Rect& camera)
{
	gDotTexture.render(mBox.x - camera.x, mBox.y - camera.y);
	renderParticles();
}

void Dot::renderParticles()
{
	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		if (particles[i]->isDead())
		{
			delete particles[i];
			particles[i] = new Particle(mBox.x, mBox.y);
		}
	}
	for (int i = 0; i < TOTAL_PARTICLES; ++i)
	{
		particles[i]->render();
	}
}

LTimer::LTimer()
{
	mStartTicks = 0;
	mPausedTicks = 0;
	mPaused = false;
	mStarted = false;
}

void LTimer::start()
{
	mStarted = true;
	mPaused = false;
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
	mStarted = false;
	mPaused = false;
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
	if (mStarted && !mPaused)
	{
		mPaused = true;
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause()
{
	if (mStarted && mPaused)
	{
		mPaused = false;
		mStartTicks = SDL_GetTicks() - mPausedTicks;
		mPausedTicks = 0;
	}
}

Uint64 LTimer::getTicks()
{
	Uint64 time = 0;
	if (mStarted)
	{
		if (mPaused)
		{
			time = mPausedTicks;
		}
		else
		{
			time = SDL_GetTicks() - mStartTicks;
		}
	}
	return time;
}

bool LTimer::isStarted()
{
	return mStarted;
}

bool LTimer::isPaused()
{
	return mPaused && mStarted;
}

LWindow::LWindow()
{
	mWindow = NULL;
	mRenderer = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init(bool bCreateRenderer)
{
#ifdef _WIN32
	if (bCreateRenderer)
	{
		if (!SDL_CreateWindowAndRenderer("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &mWindow, &mRenderer))
		{
			return false;
		}
		//SDL_SetRenderVSync(mRenderer, 1);
		SDL_SetRenderVSync(gRenderer, SDL_RENDERER_VSYNC_DISABLED);
	}
	else
	{
		mWindow = SDL_CreateWindow("SDL_Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
		if (mWindow == NULL)
		{
			return false;
		}
	}

	mMouseFocus = true;
	mKeyboardFocus = true;
	mWidth = SCREEN_WIDTH;
	mHeight = SCREEN_HEIGHT;
	return true;
#elif __linux__
	mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (mWindow != NULL)
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
	}
	return mWindow != NULL;
#endif
}

#ifdef __linux__
SDL_Renderer* LWindow::createRenderer()
{
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
}
#endif

SDL_Renderer* LWindow::getRenderer()
{
	return mRenderer;
}

void LWindow::handleEvent(SDL_Event& e)
{
	bool updateCaption = false;

	switch (e.type)
	{
#ifdef _WIN32
	case SDL_EVENT_WINDOW_RESIZED:
#elif __linux__
	case SDL_WINDOWEVENT_SIZE_CHANGED:
#endif
		mWidth = e.window.data1;
		mHeight = e.window.data2;
		SDL_RenderPresent(gRenderer);
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_EXPOSED:
#elif __linux__
	case SDL_WINDOWEVENT_EXPOSED:
#endif
		SDL_RenderPresent(gRenderer);
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MOUSE_ENTER:
#elif __linux__
	case SDL_WINDOWEVENT_ENTER:
#endif
		mMouseFocus = true;
		updateCaption = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MOUSE_LEAVE:
#elif __linux__
	case SDL_WINDOWEVENT_LEAVE:
#endif
		mMouseFocus = false;
		updateCaption = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_FOCUS_GAINED:
#elif __linux__
	case SDL_WINDOWEVENT_FOCUS_GAINED:
#endif
		mKeyboardFocus = true;
		updateCaption = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_FOCUS_LOST:
#elif __linux__
	case SDL_WINDOWEVENT_FOCUS_LOST:
#endif
		mKeyboardFocus = false;
		updateCaption = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MINIMIZED:
#elif __linux__
	case SDL_WINDOWEVENT_MINIMIZED:
#endif
		mMinimized = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MAXIMIZED:
#elif __linux__
	case SDL_WINDOWEVENT_MAXIMIZED:
#endif
		mMinimized = false;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_RESTORED:
#elif __linux__
	case SDL_WINDOWEVENT_RESTORED:
#endif
		mMinimized = false;
		break;
#ifdef _WIN32
	case SDL_EVENT_KEY_DOWN:
#elif __linux__
	case SDL_KEYDOWN:
#endif
#ifdef _WIN32
		if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_RETURN)
#elif __linux__
		if (e.key.keysym.sym == SDLK_RETURN)
#endif
		{
			if (mFullScreen)
			{
#ifdef _WIN32
				SDL_SetWindowFullscreen(mWindow, false);
#elif __linux__
				SDL_SetWindowFullscreen(mWindow, 0);
#endif
				mFullScreen = false;
			}
			else
			{
#ifdef _WIN32
				SDL_SetWindowFullscreen(mWindow, true);
#elif __linux__
				SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
				mFullScreen = true;
				mMinimized = false;
			}
		}
		break;
	}
	if (updateCaption)
	{
		std::stringstream caption;
		caption << "SDL Tutorial - MouseFocus:" << ((mMouseFocus) ? "On" : "Off") << " KeyboardFocus:" << ((mKeyboardFocus) ? "On" : "Off");
		SDL_SetWindowTitle(mWindow, caption.str().c_str());
	}
}

void LWindow::free()
{
	if (mWindow != NULL)
	{
		SDL_DestroyWindow(mWindow);
	}
	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth()
{
	return mWidth;
}

int LWindow::getHeight()
{
	return mHeight;
}

SDL_Window* LWindow::getWindow()
{
	return mWindow;
}

bool LWindow::hasMouseFocus()
{
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus()
{
	return mKeyboardFocus;
}

bool LWindow::isMinimized()
{
	return mMinimized;
}

LBitmapFont::LBitmapFont()
{
	mNewLine = 0;
	mSpace = 0;
}

bool LBitmapFont::buildFont(std::string path)
{
	free();
	bool success = true;
	if (!mFontTexture.loadPixelsFromFile(path.c_str()))
	{
		SDL_Log("Unable to load bitmap font surface!\n");
		success = false;
	}
	else
	{
		Uint32 bgColor = mFontTexture.getPixel32(0, 0);
		int cellW = mFontTexture.getWidth() / 16;
		int cellH = mFontTexture.getHeight() / 16;
		int top = cellH;
		int baseA = cellH;
		int currentChar = 0;
		for (int rows = 0; rows < 16; ++rows)
		{
			for (int cols = 0; cols < 16; ++cols)
			{
				mChars[currentChar].x = cellW * cols;
				mChars[currentChar].y = cellH * rows;
				mChars[currentChar].w = cellW;
				mChars[currentChar].h = cellH;
				for (int pCol = 0; pCol < cellW; ++pCol)
				{
					for (int pRow = 0; pRow < cellH; ++pRow)
					{
						int pX = (cellW * cols) + pCol;
						int pY = (cellH * rows) + pRow;
						if (mFontTexture.getPixel32(pX, pY) != bgColor)
						{
							mChars[currentChar].x = pX;
							pCol = cellW;
							pRow = cellH;
						}
					}
				}
				for (int pColW = cellW - 1; pColW >= 0; --pColW)
				{
					for (int pRowW = 0; pRowW < cellH; ++pRowW)
					{
						int pX = (cellW * cols) + pColW;
						int pY = (cellH * rows) + pRowW;
						if (mFontTexture.getPixel32(pX, pY) != bgColor)
						{
							mChars[currentChar].w = (pX - mChars[currentChar].x) + 1;
							pColW = -1;
							pRowW = cellH;
						}
					}
				}
				for (int pRow = 0; pRow < cellH; ++pRow)
				{
					for (int pCol = 0; pCol < cellW; ++pCol)
					{
						int pX = (cellW * cols) + pCol;
						int pY = (cellH * rows) + pRow;
						if (mFontTexture.getPixel32(pX, pY) != bgColor)
						{
							if (pRow < top)
							{
								top = pRow;
							}
							pCol = cellW;
							pRow = cellH;
						}
					}
				}
				if (currentChar == 'A')
				{
					for (int pRow = cellH - 1; pRow >= 0; --pRow)
					{
						for (int pCol = 0; pCol < cellW; ++pCol)
						{
							int pX = (cellW * cols) + pCol;
							int pY = (cellH * rows) + pRow;
							if (mFontTexture.getPixel32(pX, pY) != bgColor)
							{
								baseA = pRow;
								pCol = cellW;
								pRow = -1;
							}
						}
					}
				}
				++currentChar;
			}
		}
		mSpace = cellW / 2;
		mNewLine = baseA - top;
		for (int i = 0; i < 256; ++i)
		{
			mChars[i].y += top;
			mChars[i].h -= top;
		}
		if (!mFontTexture.loadFromPixels())
		{
			SDL_Log("Unable to create font texture!\n");
			success = false;
		}
	}

	return success;
}

void LBitmapFont::free()
{
	mFontTexture.free();
}

void LBitmapFont::renderText(int x, int y, std::string text)
{
	if (mFontTexture.getWidth() > 0)
	{
		int curX = x, curY = y;
		for (int i = 0; i < text.length(); ++i)
		{
			if (text[i] == ' ')
			{
				curX += mSpace;
			}
			else if (text[i] == '\n')
			{
				curY += mNewLine;
				curX = x;
			}
			else
			{
				int ascii = (unsigned char)text[i];
				mFontTexture.render(curX, curY, &mChars[ascii]);
				curX += mChars[ascii].w + 1;
			}
		}
	}
}

DataStream::DataStream()
{
	mImages[0] = NULL;
	mImages[1] = NULL;
	mImages[2] = NULL;
	mImages[3] = NULL;
	mCurrentImage = 0;
	mDelayFrames = 4;
}

bool DataStream::loadMedia()
{
	Load* load = new Load(SDL_GetBasePath());
	bool success = true;
	for (int i = 0; i < 4; ++i)
	{
		std::stringstream path;
		path << "foo_walk_" << i << ".png";
		//SDL_Surface* loadedSurface = IMG_Load(path.str().c_str());
		SDL_Surface* loadedSurface = IMG_Load(load->Path(path.str().c_str()));
		if (loadedSurface == NULL)
		{
			SDL_Log("Unable to load %s! SDL_image error: %s\n", path.str().c_str(), SDL_GetError());
			success = false;
		}
		else
		{
#ifdef _WIN32
			mImages[i] = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA8888);
#elif __linux__
			mImages[i] = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
#endif
		}
#ifdef _WIN32
		SDL_DestroySurface(loadedSurface);
#elif __linux__
		SDL_FreeSurface(loadedSurface);
#endif
	}
	delete load;
	return success;
}

void DataStream::free()
{
	for (int i = 0; i < 4; ++i)
	{
#ifdef _WIN32
		SDL_DestroySurface(mImages[i]);
#elif __linux__
		SDL_FreeSurface(mImages[i]);
#endif
		mImages[i] = NULL;
	}
}

void* DataStream::getBuffer()
{
	--mDelayFrames;
	if (mDelayFrames == 0)
	{
		++mCurrentImage;
		mDelayFrames = 4;
	}
	if (mCurrentImage == 4)
	{
		mCurrentImage = 0;
	}
	return mImages[mCurrentImage]->pixels;
}

bool init()
{
	bool success = true;

#ifdef _WIN32
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO))
#elif __linux__
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
#endif
	{
		SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
#ifdef _WIN32
		SDL_JoystickID joystickID = 0;
		SDL_JoystickID* joysticks = SDL_GetJoysticks(NULL);
		if (joysticks)
		{
			joystickID = joysticks[0];
			SDL_free(joysticks);
		}
		if (joystickID == 0)
		{
			SDL_Log("Warning: No joysticks connected!\n");
		}
		else
		{
			if (!SDL_IsGamepad(joystickID))
			{
				SDL_Log("Warning: Joystick is not game controller interface compatible! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				gGameController = SDL_OpenGamepad(joystickID);
				if (!SDL_GetBooleanProperty(SDL_GetGamepadProperties(gGameController), SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false))
				{
					SDL_Log("Warning: Game controller does not have rumble! SDL Error: %s\n", SDL_GetError());
				}
			}
			if (gGameController == NULL)
			{
				gJoystick = SDL_OpenJoystick(joystickID);
				if (gJoystick == NULL)
				{
					SDL_Log("Warning: Unable to open joystick! SDL Error: %s\n", SDL_GetError());
				}
				else
				{
					if (!SDL_GetBooleanProperty(SDL_GetJoystickProperties(gJoystick), SDL_PROP_JOYSTICK_CAP_RUMBLE_BOOLEAN, false))
					{

					}
					if (!SDL_IsJoystickHaptic(gJoystick))
					{
						SDL_Log("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError());
					}
					else
					{
						gJoyHaptic = SDL_OpenHapticFromJoystick(gJoystick);
						if (gJoyHaptic == NULL)
						{
							SDL_Log("Warning: Unable to get joystick haptics! SDL Error: %s\n", SDL_GetError());
						}
						else
						{
							if (!SDL_InitHapticRumble(gJoyHaptic))
							{
								SDL_Log("Warning: Unable to initialize haptic rumble! SDL Error: %s\n", SDL_GetError());
							}
						}
					}
				}
			}
		}
#elif __linux__
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!\n");
		}
		if (SDL_NumJoysticks() < 1)
		{
			printf("Warning: No joysticks connected!\n");
		}
		else
		{
			if (!SDL_IsGameController(0))
			{
				printf("Warning: joystick is not game controller interface compatible! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				gGameController = SDL_GameControllerOpen(0);
				if (!SDL_GameControllerHasRumble(gGameController))
				{
					printf("Warning: Game controller does not have rumble! SDL Error: %s\n", SDL_GetError());
				}
			}
			if (gGameController == NULL)
			{
				gJoystick = SDL_JoystickOpen(0);
				if (gJoystick == NULL)
				{
					printf("Warning: Unable to open joystick! SDL Error: %s\n", SDL_GetError());
				}
				else
				{
					if (!SDL_JoystickIsHaptic(gJoystick))
					{
						printf("Warning: Controller does not support haptics! SDL Error: %s\n", SDL_GetError());
					}
					else
					{
						gJoyHaptic = SDL_HapticOpenFromJoystick(gJoystick);
						if (gJoyHaptic == NULL)
						{
							printf("Warning: Unable to get joystick haptics! SDL Error: %s\n", SDL_GetError());
						}
						else
						{
							if (SDL_HapticRumbleInit(gJoyHaptic) < 0)
							{
								printf("Warning: Unable to initialize haptic rumble! SDL Error: %s\n", SDL_GetError());
							}
						}
					}
				}
			}
		}
#endif
		srand(SDL_GetTicks());
#ifdef _WIN32
		//if (!SDL_CreateWindowAndRenderer("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &gWindow, &gRenderer))
		if (!gWindow.init(true))
#elif __linux__
		//gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (!gWindow.init(true))
#endif
		{
			SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
#ifdef _WIN32
			gRenderer = gWindow.getRenderer();
			//SDL_SetRenderVSync(gRenderer, 1);
			SDL_SetRenderVSync(gRenderer, SDL_RENDERER_VSYNC_DISABLED);
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
#elif __linux__
			//gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			gRenderer = gWindow.createRenderer();
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
#endif
#ifdef _WIN32
			if (!TTF_Init())
#elif __linux__
			if (TTF_Init() == -1)
#endif
			{
				SDL_Log("SDL_ttf could not initialize! SDL_ttf Error: %s\n", SDL_GetError());
				success = false;
			}
#ifdef _WIN32
			if (!MIX_Init())
			{
				printf("MIX_Init failed");
			}
			//SDL_zero(audio_spec);
			//audio_spec.format = SDL_AUDIO_F32;
			//audio_spec.channels = 2;
			//audio_spec.freq = 48000;
			//gMusic = MIX_CreateMixerDevice(0, NULL);
			//gMusic = MIX_CreateMixer(NULL);
			//if (gMusic == NULL)
			//{
			//	SDL_Log("Could not create mixer: %s\n", SDL_GetError());
			//}
			//gMusic = MIX_CreateMixerDevice(0, nullptr);
			//if (gMusic != 0) {
			//	SDL_Log("Unable to initialize SDL_mixer\n");
			//	SDL_Quit();
			//	return 1;
			//}
			SDL_AudioSpec spec;
			spec.channels = 1;
			spec.format = SDL_AUDIO_F32;
			spec.freq = 8000;
			stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
			if (!stream) {
				SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
				return SDL_APP_FAILURE;
			}
			/* SDL_OpenAudioDeviceStream starts the device paused. You have to tell it to start! */
			SDL_ResumeAudioStreamDevice(stream);
#elif __linux__
			if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
			{
				printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
				success = false;
			}
#endif
			//gScreenSurface = SDL_GetWindowSurface(gWindow);
			gScreenSurface = SDL_GetWindowSurface(gWindow.getWindow());
		}
	}

	return success;
}

bool loadMedia()
{
	using namespace std;

	Load* load = new Load(SDL_GetBasePath());
	bool success = true;

	SDL_Color textColor = { 0, 0, 0, 0xFF };
	SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };

	gTexture = loadTexture(load->Path("Whitebox_Square_1024x1024_000.png"), load, &success);
	if (!gModulatedTexture.loadFromFile(load->Path("Landscape_Moon_3300x2550.png")))
	{
		SDL_Log("Failed to load front texture!\n");
		success = false;
	}
	else
	{
		gModulatedTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	}
	if (!gWalkingSpriteSheetTexture.loadFromFile(load->Path("foo.png")))
	{
		SDL_Log("Failed to load walking animation texture!\n");
		success = false;
	}
	else
	{
		gWalkingSpriteClips[0].x = 0;
		gWalkingSpriteClips[0].y = 0;
		gWalkingSpriteClips[0].w = 64;
		gWalkingSpriteClips[0].h = 205;
		gWalkingSpriteClips[1].x = 64;
		gWalkingSpriteClips[1].y = 0;
		gWalkingSpriteClips[1].w = 64;
		gWalkingSpriteClips[1].h = 205;
		gWalkingSpriteClips[2].x = 128;
		gWalkingSpriteClips[2].y = 0;
		gWalkingSpriteClips[2].w = 64;
		gWalkingSpriteClips[2].h = 205;
		gWalkingSpriteClips[3].x = 192;
		gWalkingSpriteClips[3].y = 0;
		gWalkingSpriteClips[3].w = 64;
		gWalkingSpriteClips[3].h = 205;
	}
	if (!gIconCursor.loadFromFile(load->Path("IconCursor.png")))
	{
		SDL_Log("Failed to load arrow texture!\n");
		success = false;
	}
	if (!gSun.loadFromFile(load->Path("CharacterFairySun_000_256x256.png")))
	{
		SDL_Log("Failed to load CharacterFairySun_000_256x256 texture!\n");
		success = false;
	}
	if (!gMoon.loadFromFile(load->Path("CharacterFairyMoon_000_256x256.png")))
	{
		SDL_Log("Failed to load CharacterFairyMoon_000_256x256 texture!\n");
		success = false;
	}
#ifdef _WIN32
	SDL_IOStream* file = SDL_IOFromFile("nums.bin", "r+b");
#elif __linux__
	SDL_RWops* file = SDL_RWFromFile(load->Path("nums.bin"), "r+b");
#endif
	if (file == NULL)
	{
		SDL_Log("Warning: Unable to open file! SDL Error: %s\n", SDL_GetError());
#ifdef _WIN32
		file = SDL_IOFromFile("nums.bin", "w+b");
#elif __linux__
		file = SDL_RWFromFile(load->Path("nums.bin"), "w+b");
#endif
		if (file != NULL)
		{
			SDL_Log("New file created!\n");
			for (int i = 0; i < TOTAL_DATA; ++i)
			{
				gData[i] = 0;
#ifdef _WIN32
				SDL_WriteIO(file, &gData[i], sizeof(Sint32));
#elif __linux__
				SDL_RWwrite(file, &gData[i], sizeof(Sint32), 1);
#endif
			}
#ifdef _WIN32
			SDL_CloseIO(file);
#elif __linux__
			SDL_RWclose(file);
#endif
		}
		else
		{
			SDL_Log("Error: Unable to create file! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
	}
	else
	{
		SDL_Log("Reading file...!\n");
		for (int i = 0; i < TOTAL_DATA; ++i)
		{
#ifdef _WIN32
			SDL_ReadIO(file, &gData[i], sizeof(Sint32));
#elif __linux__
			SDL_RWread(file, &gData[i], sizeof(Sint32), 1);
#endif
		}
#ifdef _WIN32
		SDL_CloseIO(file);
#elif __linux__
		SDL_RWclose(file);
#endif
	}
	if (!gButtonSpriteSheetTexture.loadFromFile(load->Path("button.png")))
	{
		SDL_Log("Failed to load button sprite texture!\n");
		success = false;
	}
	else
	{
		for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i)
		{
			gSpriteClips[i].x = 0;
			gSpriteClips[i].y = i * 200;
			gSpriteClips[i].w = BUTTON_WIDTH;
			gSpriteClips[i].h = BUTTON_HEIGHT;
		}
		gButtons[0].setPosition(0, 0);
		gButtons[1].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, 0);
		gButtons[2].setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);
		gButtons[3].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT);
	}
	/*
	//gMusic = Mix_LoadMUS(load->Path("beat.wav"));
	gBeat = MIX_LoadAudio(gMusic, load->Path("beat.wav"), false);
	if (gBeat == NULL)
	{
		SDL_Log("Failed to load beat music! SDL_mixer Error: %s\n", SDL_GetError());
		success = false;
	}
	gScratch = MIX_LoadAudio(gMusic, load->Path("scratch.wav"), false);
	if (gScratch == NULL)
	{
		SDL_Log("Failed to load scratch music! SDL_mixer Error: %s\n", SDL_GetError());
		success = false;
	}
	gHigh = MIX_LoadAudio(gMusic, load->Path("high.wav"), false);
	if (gHigh == NULL)
	{
		SDL_Log("Failed to load high music! SDL_mixer Error: %s\n", SDL_GetError());
		success = false;
	}
	gMedium = MIX_LoadAudio(gMusic, load->Path("medium.wav"), false);
	if (gMedium == NULL)
	{
		SDL_Log("Failed to load medium music! SDL_mixer Error: %s\n", SDL_GetError());
		success = false;
	}
	gLow = MIX_LoadAudio(gMusic, load->Path("low.wav"), false);
	if (gLow == NULL)
	{
		SDL_Log("Failed to load low music! SDL_mixer Error: %s\n", SDL_GetError());
		success = false;
	}*/

	if (!gDotTexture.loadFromFile(load->Path("CharacterFairySmallwig000_64x64.png")))
	{
		SDL_Log("Failed to load dot texture!\n");
		success = false;
	}
	if (!gBGTexture.loadFromFile(load->Path("bg.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[0].loadFromFile(load->Path("Horizon000.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[1].loadFromFile(load->Path("Horizon001.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[2].loadFromFile(load->Path("Horizon002.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[3].loadFromFile(load->Path("Horizon003.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[4].loadFromFile(load->Path("Horizon004.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[5].loadFromFile(load->Path("Horizon005.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[6].loadFromFile(load->Path("Horizon006.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gHorizons[7].loadFromFile(load->Path("Horizon007.png")))
	{
		SDL_Log("Failed to load background texture~\n");
		success = false;
	}
	if (!gBoxes[KEY_PRESS_SURFACE_UP].loadFromFile(load->Path("BoxUp.png")))
	{
		SDL_Log("Failed to load BoxUp texture~\n");
		success = false;
	}
	if (!gBoxes[KEY_PRESS_SURFACE_DOWN].loadFromFile(load->Path("BoxDown.png")))
	{
		SDL_Log("Failed to load BoxDown texture~\n");
		success = false;
	}
	if (!gBoxes[KEY_PRESS_SURFACE_LEFT].loadFromFile(load->Path("BoxLeft.png")))
	{
		SDL_Log("Failed to load BoxLeft texture~\n");
		success = false;
	}
	if (!gBoxes[KEY_PRESS_SURFACE_RIGHT].loadFromFile(load->Path("BoxRight.png")))
	{
		SDL_Log("Failed to load BoxRight texture~\n");
		success = false;
	}


	if (!gRedTexture.loadFromFile(load->Path("CharacterFairySmallcol000_64x64.png")))
	{
		SDL_Log("Failed to load red texture!\n");
		success = false;
	}
	if (!gGreenTexture.loadFromFile(load->Path("CharacterFairySmalling000_64x64.png")))
	{
		SDL_Log("Failed to load green texture!\n");
		success = false;
	}
	if (!gBlueTexture.loadFromFile(load->Path("CharacterFairySmallbin000_64x64.png")))
	{
		SDL_Log("Failed to load blue texture!\n");
		success = false;
	}
	if (!gShimmerTexture.loadFromFile(load->Path("CharacterFairySmallton000_64x64.png")))
	{
		SDL_Log("Failed to load shimmer texture!\n");
		success = false;
	}
	gRedTexture.setAlpha(192);
	gGreenTexture.setAlpha(192);
	gBlueTexture.setAlpha(192);
	gShimmerTexture.setAlpha(192);
	if (!gBitmapFont.buildFont(load->Path("font_000.png")))
	{
		SDL_Log("Failed to load bitmap font!\n");
		success = false;
	}
	if (!gTargetTexture.createBlank(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_TEXTUREACCESS_TARGET))
	{
		SDL_Log("Failed to create target texture!\n");
		success = false;
	}

	delete load;
	return success;
}

void close()
{
	Load* load = new Load(SDL_GetBasePath());

	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
	gModulatedTexture.free();
	gWalkingSpriteSheetTexture.free();
	gIconCursor.free();
	gSun.free();
	gMoon.free();
	gButtonSpriteSheetTexture.free();
	if (gGameController != NULL)
	{
#ifdef _WIN32
		SDL_CloseGamepad(gGameController);
#elif __linux__
		SDL_GameControllerClose(gGameController);
#endif
	}
	if (gJoyHaptic != NULL)
	{
#ifdef _WIN32
		SDL_CloseHaptic(gJoyHaptic);
#elif __linux__
		SDL_HapticClose(gJoyHaptic);
#endif
	}
	if (gJoystick != NULL)
	{
#ifdef _WIN32
		SDL_CloseJoystick(gJoystick);
#elif __linux__
		SDL_JoystickClose(gJoystick);
#endif
	}
	gGameController = NULL;
	gJoystick = NULL;
	gJoyHaptic = NULL;
	//MIX_DestroyAudio(gBeat);
	//MIX_DestroyAudio(gScratch);
	//MIX_DestroyAudio(gHigh);
	//MIX_DestroyAudio(gMedium);
	//MIX_DestroyAudio(gLow);
	//gBeat = NULL;
	//gScratch = NULL;
	//gHigh = NULL;
	//gMedium = NULL;
	//gLow = NULL;
	//MIX_DestroyMixer(gMusic);
	//gMusic = NULL;
	gDotTexture.free();
	gBGTexture.free();
	for (int i = 0; i < HORIZON_SIZE; ++i)
	{
		gHorizons[i].free();
	}
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
	{
		gBoxes[i].free();
	}
#ifdef _WIN32
	SDL_IOStream* file = SDL_IOFromFile("nums.bin", "w+b");
#elif __linux__
	SDL_RWops* file = SDL_RWFromFile(load->Path("nums.bin"), "w+b");
#endif
	if (file != NULL)
	{
		for (int i = 0; i < TOTAL_DATA; ++i)
		{
#ifdef _WIN32
			SDL_WriteIO(file, &gData[i], sizeof(Sint32));
#elif __linux__
			SDL_RWwrite(file, &gData[i], sizeof(Sint32), 1);
#endif
		}
#ifdef _WIN32
		SDL_CloseIO(file);
#elif __linux__
		SDL_RWclose(file);
#endif
	}
	else
	{
		SDL_Log("Error: Unable to save file!\n", SDL_GetError());
	}
	gRedTexture.free();
	gGreenTexture.free();
	gBlueTexture.free();
	gShimmerTexture.free();
	gBitmapFont.free();
	gTargetTexture.free();

	delete load;
	SDL_DestroyRenderer(gRenderer);
	gWindow.free();
	TTF_Quit();
#ifdef _WIN32
	MIX_Quit();
#elif __linux__
	Mix_Quit();
#endif
	SDL_Quit();
}

SDL_Surface* loadSurface(const char* path, Load* load, bool* success)
{
	SDL_Surface* optimizedSurface = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	load->Print(loadedSurface, path);
	if (loadedSurface == NULL)
	{
		SDL_Log("Unable to load image %s! SDL Error: %s\n", path, SDL_GetError());
		(*success) = false;
	}
	else
	{
#ifdef _WIN32
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format);
#elif __linux__
		optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
#endif
		if (optimizedSurface == NULL)
		{
			SDL_Log("Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError());
		}
#ifdef _WIN32
		SDL_DestroySurface(loadedSurface);
#elif __linux__
		SDL_FreeSurface(loadedSurface);
#endif
	}

	return optimizedSurface;
}

SDL_Texture* loadTexture(const char* path, Load* load, bool* success)
{
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	load->Print(loadedSurface, path);
	if (loadedSurface == NULL)
	{
		SDL_Log("Unable to load image %s! SDL_image: %s\n", path, SDL_GetError());
		(*success) = false;
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			SDL_Log("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}
#ifdef _WIN32
		SDL_DestroySurface(loadedSurface);
#elif __linux__
		SDL_FreeSurface(loadedSurface);
#endif
	}

	return newTexture;
}

int main(int argc, char* argv[])
{
	Clock* clock = new Clock();
	Timer* myTimer = new Timer(Timer::Print, 1);

	if (!init())
	{
		SDL_Log("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia())
		{
			SDL_Log("Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			SDL_Event e;
			int frame = 0;
			double degrees = 0;
#ifdef _WIN32
			SDL_FlipMode flipType = SDL_FLIP_NONE;
#elif __linux__
			SDL_RendererFlip flipType = SDL_FLIP_NONE;
#endif
			int xDir = 0;
			int yDir = 0;
			SDL_Color textColor = { 0, 0, 0, 255 };
			Uint64 startTime = 0;
			LTimer fpsTimer;
			LTimer capTimer;
			int countedFrames = 0;
			fpsTimer.start();
			Dot dot;
//#ifdef _WIN32
//			SDL_FRect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
//#elif __linux__
			SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
			//#endif
#ifdef _WIN32
			SDL_FRect fwall;
#elif __linux__
			SDL_Rect fwall;
#endif
			SDL_Rect wall;
			wall.x = 300;
			wall.y = 40;
			wall.w = 40;
			wall.h = 400;
			int backgroundScrollingOffset = 0;
			int horizonScrollingOffset = 0;
			std::string inputText = "Some Text";
			int currentData = 0;
			SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };
#ifdef _WIN32
			SDL_StartTextInput(gWindow.getWindow());
#elif __linux__
			SDL_StartTextInput();
#endif
			double minuteAngle = 0;
			double hourAngle = 0;
			double halfDayAngle = 0;
			double fullDayAngle = 0;
#ifdef _WIN32
			SDL_FPoint screenCenter = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
#elif __linux__
			SDL_Point screenCenter = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
#endif
			bool isDebug = false;
			bool isDebugReleased = false;
#ifdef _WIN32
			float mouseX, mouseY;
#elif __linux__
			int mouseX, mouseY;
#endif

			long long previousTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			long long secondDelta = 0L;
			long long minuteDelta = 0L;
			long long hourDelta = 0L;
			long long halfDayDelta = 0L;
			long long fullDayDelta = 0L;
			while (!quit)
			{
				long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				secondDelta += currentTime - previousTime;
				minuteDelta += currentTime - previousTime;
				hourDelta += currentTime - previousTime;
				halfDayDelta += currentTime - previousTime;
				fullDayDelta += currentTime - previousTime;
				double secondLimit = 1'000'000'000;
				double minuteLimit = 60'000'000'000;
				double hourLimit = 3'600'000'000'000;
				double halfDayLimit = 43'200'000'000'000;
				double fullDayLimit = 86'400'000'000'000;
				if (secondDelta > secondLimit)
				{
					secondDelta -= secondLimit;
				}
				if (minuteDelta > minuteLimit)
				{
					minuteDelta -= minuteLimit;
				}
				if (hourDelta > hourLimit)
				{
					hourDelta -= hourLimit;
				}
				if (halfDayDelta > halfDayLimit)
				{
					halfDayDelta -= halfDayLimit;
				}
				if (fullDayDelta > fullDayLimit)
				{
					fullDayDelta -= fullDayLimit;
				}
				double secondNormal = (double)secondDelta / secondLimit;
				double minuteNormal = (double)minuteDelta / minuteLimit;
				double hourNormal = (double)hourDelta / hourLimit;
				double halfDayNormal = (double)halfDayDelta / halfDayLimit;
				double fullDayNormal = (double)fullDayDelta / fullDayLimit;
				previousTime = currentTime;

				SDL_GetMouseState(&mouseX, &mouseY);
#ifdef _WIN32
				const int minimum_audio = (8000 * sizeof(float)) / 2;  /* 8000 float samples per second. Half of that. */
				if (SDL_GetAudioStreamQueued(stream) < minimum_audio) {
					static float samples[512];  /* this will feed 512 samples each frame until we get to our maximum. */
					int i;

					/* generate a 440Hz pure tone */
					for (i = 0; i < SDL_arraysize(samples); i++) {
						const int freq = 440;
						const float phase = current_sine_sample * freq / 8000.0f;
						samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
						current_sine_sample++;
					}

					/* wrapping around to avoid floating-point errors */
					current_sine_sample %= 8000;

					/* feed the new data to the stream. It will queue at the end, and trickle out as the hardware needs more data. */
					SDL_PutAudioStreamData(stream, samples, sizeof(samples));
				}
#elif __linux__
				SDL_AudioFormat src_format = AUDIO_S16;
				Uint8 src_channels = 1;
				int src_rate = 22050;
				SDL_AudioFormat dst_format = AUDIO_F32;
				Uint8 dst_channels = 2;
				int dst_rate = 48000;
				stream = SDL_NewAudioStream(src_format, src_channels, src_rate, dst_format, dst_channels, dst_rate);
				if (stream == NULL) {
					fprintf(stderr, "Failed to create audio stream: %s\n", SDL_GetError());
					SDL_Quit();
					return 1;
				}
				const int num_src_samples = 1024;
				Sint16 src_audio_buffer[num_src_samples];
				for (int i = 0; i < num_src_samples; ++i)
				{
					src_audio_buffer[i] = (Sint16)(30000 * sin(i * M_PI / 100.0));
				}
				int put_result = SDL_AudioStreamPut(stream, src_audio_buffer, num_src_samples * sizeof(Sint16));
				if (put_result == -1)
				{
					fprintf(stderr, "Failed to put data into audio stream: %s\n", SDL_GetError());
					SDL_FreeAudioStream(stream);
					SDL_Quit();
					return 1;
				}
				//printf("Put %d bytes of source audio into the stream.\n", num_src_samples * sizeof(Sint16));
				const int num_dst_samples_estimate = (int)(num_src_samples * (double)dst_rate / src_rate * dst_channels / src_channels);
				float dst_audio_buffer[num_dst_samples_estimate];
				int available_bytes = SDL_AudioStreamAvailable(stream);
				//printf("Available converted bytes in stream: %d\n", available_bytes);
				int get_result = SDL_AudioStreamGet(stream, dst_audio_buffer, available_bytes);
				if (get_result == -1)
				{
					fprintf(stderr, "Failed to get data from audio stream: %s\n", SDL_GetError());
					SDL_FreeAudioStream(stream);
					SDL_Quit();
					return 1;
				}
				//printf("Got %d bytes of converted audio from the stream.\n", get_result);
#endif
				myTimer->Update();
				capTimer.start();
				while (SDL_PollEvent(&e) != 0)
				{
					if (IsWindowQuit(e)) quit = true;
#ifdef _WIN32
					else if (e.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN)
#elif __linux__
					else if (e.type == SDL_JOYBUTTONDOWN)
#endif
					{
						if (gGameController != NULL)
						{
#ifdef _WIN32
							if (!SDL_RumbleGamepad(gGameController, 0xFFFF * 3 / 4, 0xFFFF * 3 / 4, 500))
#elif __linux__
							if (SDL_GameControllerRumble(gGameController, 0xFFFF * 3 / 4, 0xFFFF * 3 / 4, 500) != 0)
#endif
							{
								SDL_Log("Warning: Unable to play game controller rumble! %s\n", SDL_GetError());
							}
						}
						else if (gJoyHaptic != NULL)
						{
#ifdef _WIN32
							if (!SDL_PlayHapticRumble(gJoyHaptic, 0.75, 500))
#elif __linux__
							if (SDL_HapticRumblePlay(gJoyHaptic, 0.75, 500) != 0)
#endif
							{
								SDL_Log("Warning: Unable to play haptic rumble! %s\n", SDL_GetError());
							}
						}
					}
#ifdef _WIN32
					else if (e.type == SDL_EVENT_JOYSTICK_AXIS_MOTION)
#elif __linux__
					else if (e.type == SDL_JOYAXISMOTION)
#endif
					{
						//if (e.jaxis.which == 0)
						//{
						if (e.jaxis.axis == 0)
						{
							if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
							{
								xDir = -1;
							}
							else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
							{
								xDir = 1;
							}
							else
							{
								xDir = 0;
							}
						}
						else if (e.jaxis.axis == 1)
						{
							if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
							{
								yDir = -1;
							}
							else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
							{
								yDir = 1;
							}
							else
							{
								yDir = 0;
							}
						}
						//}
						//printf("%i, %i\n", xDir, yDir);
					}
#ifdef _WIN32
					else if (e.type == SDL_EVENT_KEY_UP)
#elif __linux__
					else if (e.type == SDL_KEYUP)
#endif
					{
#ifdef _WIN32
						switch (e.key.key)
#elif __linux__
						switch (e.key.keysym.sym)
#endif
						{
						case SDLK_HOME:
							break;
						}
					}
#ifdef _WIN32
					else if (e.type == SDL_EVENT_KEY_DOWN)
#elif __linux__
					else if (e.type == SDL_KEYDOWN)
#endif
					{
#ifdef _WIN32
						switch (e.key.key)
#elif __linux__
						switch (e.key.keysym.sym)
#endif
						{
						case SDLK_HOME:
							isDebug = !isDebug;
							break;
						case SDLK_END:
							switch (gDirection)
							{
							case DIRECTION_UP:
								gDirection = DIRECTION_DOWN;
								break;
							case DIRECTION_DOWN:
								gDirection = DIRECTION_LEFT;
								break;
							case DIRECTION_LEFT:
								gDirection = DIRECTION_RIGHT;
								break;
							case DIRECTION_RIGHT:
								gDirection = DIRECTION_UP;
								break;
							}
							break;
						case SDLK_UP:
						case KEY_W:
							gBox = &gBoxes[KEY_PRESS_SURFACE_UP];
							--currentData;
							if (currentData < 0)
							{
								currentData = TOTAL_DATA - 1;
							}
							break;
						case SDLK_DOWN:
						case KEY_S:
							gBox = &gBoxes[KEY_PRESS_SURFACE_DOWN];
							++currentData;
							if (currentData == TOTAL_DATA)
							{
								currentData = 0;
							}
							break;
						case SDLK_LEFT:
						case KEY_A:
							gBox = &gBoxes[KEY_PRESS_SURFACE_LEFT];
							--gData[currentData];
							degrees -= 60;
							break;
						case SDLK_RIGHT:
						case KEY_D:
							gBox = &gBoxes[KEY_PRESS_SURFACE_RIGHT];
							++gData[currentData];
							degrees += 60;
							break;
						default:
							break;
						}
					}

#ifdef _WIN32
					if (e.type == SDL_EVENT_KEY_DOWN)
#elif __linux__
					if (e.type == SDL_KEYDOWN)
#endif
					{
#ifdef _WIN32
						if (e.key.key == SDLK_BACKSPACE && inputText.length() > 0)
#elif __linux__
						if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
#endif
						{
							inputText.pop_back();
						}
#ifdef _WIN32
						else if (e.key.key == SDLK_C && SDL_GetModState() & SDL_KMOD_CTRL)
#elif __linux__
						else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
#endif
						{
							SDL_SetClipboardText(inputText.c_str());
						}
#ifdef _WIN32
						else if (e.key.key == SDLK_V && SDL_GetModState() & SDL_KMOD_CTRL)
#elif __linux__
						else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
#endif
						{
							char* tempText = SDL_GetClipboardText();
							inputText = tempText;
							SDL_free(tempText);
						}
					}
#ifdef _WIN32
					else if (e.type == SDL_EVENT_TEXT_INPUT)
#elif __linux__
					else if (e.type == SDL_TEXTINPUT)
#endif
					{
#ifdef _WIN32
						if (!(SDL_GetModState() & SDL_KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
#elif __linux__
						if (!(SDL_GetModState() & KMOD_CTRL && (e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
#endif
						{
							inputText += e.text.text;
						}
					}
					for (int i = 0; i < TOTAL_BUTTONS; ++i)
					{
						gButtons[i].handleEvent(&e);
					}
					dot.handleEvent(e);
					gWindow.handleEvent(e);
				}
				if (!gWindow.isMinimized())
				{
					SDL_RenderClear(gRenderer);

					frame = 4 * secondNormal;
#ifdef _WIN32
					SDL_FRect* currentClip = &gWalkingSpriteClips[frame];
#elif __linux__
					SDL_Rect* currentClip = &gWalkingSpriteClips[frame];
#endif

					double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);
					if (xDir == 0 && yDir == 0)
					{
						joystickAngle = 0;
					}

					switch (gDirection)
					{
					case DIRECTION_UP:
						backgroundScrollingOffset = -gBGTexture.getHeight() * minuteNormal;
						if (backgroundScrollingOffset < -gBGTexture.getHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case DIRECTION_DOWN:
						backgroundScrollingOffset = gBGTexture.getHeight() * minuteNormal;
						if (backgroundScrollingOffset > gBGTexture.getHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case DIRECTION_LEFT:
						backgroundScrollingOffset = -gBGTexture.getWidth() * minuteNormal;
						if (backgroundScrollingOffset < -gBGTexture.getWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case DIRECTION_RIGHT:
						backgroundScrollingOffset = gBGTexture.getWidth() * minuteNormal;
						if (backgroundScrollingOffset > gBGTexture.getWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					}
					horizonScrollingOffset = (- gHorizons[0].getWidth() * HORIZON_SIZE) * minuteNormal;
					if (horizonScrollingOffset < -gHorizons[0].getWidth() * HORIZON_SIZE)
					{
						horizonScrollingOffset = 0;
					}

					minuteAngle = 360 * minuteNormal;
					hourAngle = 360 * hourNormal;
					halfDayAngle = 360 * halfDayNormal;
					fullDayAngle = 360 * fullDayNormal;

#ifdef _WIN32
					SDL_FRect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
#elif __linux__
					SDL_Rect fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
#endif
#ifdef _WIN32
					SDL_FRect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
#elif __linux__
					SDL_Rect outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
#endif


					switch (gDirection)
					{
					case DIRECTION_UP:
						gBGTexture.render(0, backgroundScrollingOffset);
						gBGTexture.render(0, backgroundScrollingOffset + gBGTexture.getHeight());
						gBGTexture.render(0, backgroundScrollingOffset + (gBGTexture.getHeight() * 2));
						gBGTexture.render(gBGTexture.getWidth(), backgroundScrollingOffset);
						gBGTexture.render(gBGTexture.getWidth(), backgroundScrollingOffset + gBGTexture.getHeight());
						gBGTexture.render(gBGTexture.getWidth(), backgroundScrollingOffset + (gBGTexture.getHeight() * 2));
						break;
					case DIRECTION_DOWN:
						gBGTexture.render(0, backgroundScrollingOffset);
						gBGTexture.render(0, backgroundScrollingOffset + gBGTexture.getHeight());
						gBGTexture.render(0, backgroundScrollingOffset - gBGTexture.getHeight());
						gBGTexture.render(gBGTexture.getWidth(), backgroundScrollingOffset);
						gBGTexture.render(gBGTexture.getWidth(), backgroundScrollingOffset + gBGTexture.getHeight());
						gBGTexture.render(gBGTexture.getWidth(), backgroundScrollingOffset - gBGTexture.getHeight());
						break;
					case DIRECTION_LEFT:
						gBGTexture.render(backgroundScrollingOffset, 0);
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.getWidth(), 0);
						gBGTexture.render(backgroundScrollingOffset + (gBGTexture.getWidth() * 2), 0);
						gBGTexture.render(backgroundScrollingOffset, gBGTexture.getHeight());
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.getWidth(), gBGTexture.getHeight());
						gBGTexture.render(backgroundScrollingOffset + (gBGTexture.getWidth() * 2), gBGTexture.getHeight());
						break;
					case DIRECTION_RIGHT:
						gBGTexture.render(backgroundScrollingOffset, 0);
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.getWidth(), 0);
						gBGTexture.render(backgroundScrollingOffset - gBGTexture.getWidth(), 0);
						gBGTexture.render(backgroundScrollingOffset, gBGTexture.getHeight());
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.getWidth(), gBGTexture.getHeight());
						gBGTexture.render(backgroundScrollingOffset - gBGTexture.getWidth(), gBGTexture.getHeight());
						break;
					}
					for (int i = 0; i < HORIZON_SIZE; ++i)
					{
						gHorizons[i].render(horizonScrollingOffset + (gHorizons[i].getWidth() * i), 0 /*gWindow.getHeight() - gHorizons[i].getHeight()*/);
					}
					SDL_Rect fullscreenViewport;
					fullscreenViewport.x = 0;
					fullscreenViewport.y = 0;
					fullscreenViewport.w = gWindow.getWidth();
					fullscreenViewport.h = gWindow.getHeight();
					if (isDebug)
					{
						SetRenderViewport(gRenderer, &fullscreenViewport);
						gModulatedTexture.render(0, 0);
						RenderTexture(gRenderer, gModulatedTexture.getTexture());
					}
					dot.render(camera);

					SDL_Rect middleViewport;
					middleViewport.x = (gWindow.getWidth() * 0.5f) - 100;
					middleViewport.y = (gWindow.getHeight() * 0.5f) - 100;
					middleViewport.w = 200;
					middleViewport.h = 200;
					SetRenderViewport(gRenderer, &middleViewport);
					RenderTexture(gRenderer, gTexture);
					gWalkingSpriteSheetTexture.render(
						(middleViewport.w - currentClip->w) / 2,
						(middleViewport.h - currentClip->h) / 2,
						currentClip);
					middleViewport.y += middleViewport.h / 2;
					SetRenderViewport(gRenderer, &middleViewport);
					RenderTexture(gRenderer, gBox->getTexture());
					SetRenderViewport(gRenderer, &fullscreenViewport);

					gSun.render(
						0,
						0,
						NULL,
						-minuteAngle);
					gMoon.render(
						gWindow.getWidth() - gMoon.getWidth(),
						0,
						NULL,
						minuteAngle);
					gIconCursor.render(
						0,
						gWindow.getHeight() - gIconCursor.getHeight(),
						NULL,
						joystickAngle);
					gIconCursor.render(
						gWindow.getWidth() - gIconCursor.getWidth(),
						gWindow.getHeight() - gIconCursor.getHeight(),
						NULL,
						degrees,
						NULL,
						flipType);

					if (isDebug)
					{
						for (int i = 0; i < TOTAL_BUTTONS; ++i)
						{
							gButtons[i].render();
						}
						gTargetTexture.setAsRenderTarget();
						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
						SDL_RenderClear(gRenderer);
						fillRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
						SDL_RenderFillRect(gRenderer, &fillRect);
						outlineRect = { SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
						SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
						RenderRect(gRenderer, &outlineRect);
						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
						RenderLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
						RenderLine(gRenderer, outlineRect.x, outlineRect.y, mouseX, mouseY);
						RenderLine(gRenderer, outlineRect.x + outlineRect.w, outlineRect.y, mouseX, mouseY);
						RenderLine(gRenderer, outlineRect.x, outlineRect.y + outlineRect.h, mouseX, mouseY);
						RenderLine(gRenderer, outlineRect.x + outlineRect.w, outlineRect.y + outlineRect.h, mouseX, mouseY);
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
						for (int i = 0; i < SCREEN_HEIGHT; i += 4)
						{
							RenderPoint(gRenderer, SCREEN_WIDTH / 2, i);
						}
						SDL_SetRenderTarget(gRenderer, NULL);
						RenderLine(gRenderer, 0, 0, mouseX, mouseY);
						RenderLine(gRenderer, SCREEN_WIDTH, 0, mouseX, mouseY);
						RenderLine(gRenderer, 0, SCREEN_HEIGHT, mouseX, mouseY);
						RenderLine(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT, mouseX, mouseY);
						gTargetTexture.render(-SCREEN_WIDTH * 0.25f, -SCREEN_HEIGHT * 0.25f, NULL, fullDayAngle, &screenCenter);
						gTargetTexture.render(-SCREEN_WIDTH * 0.25f, -SCREEN_HEIGHT * 0.25f, NULL, halfDayAngle, &screenCenter);
						gTargetTexture.render(-SCREEN_WIDTH * 0.25f, -SCREEN_HEIGHT * 0.25f, NULL, hourAngle, &screenCenter);
						gTargetTexture.render(-SCREEN_WIDTH * 0.25f, -SCREEN_HEIGHT * 0.25f, NULL, minuteAngle, &screenCenter);
						if (inputText != "")
						{
							gBitmapFont.renderText(0, gWindow.getHeight() - 128, inputText.c_str());
						}
						else
						{
							gBitmapFont.renderText(0, gWindow.getHeight() - 128, "");
						}
						gBitmapFont.renderText(0, 0, std::to_string(gData[currentData]).c_str());
					}
					SDL_RenderPresent(gRenderer);
				}
				++countedFrames;

				//int frameTicks = capTimer.getTicks();
				//if (frameTicks < SCREEN_TICK_PER_FRAME)
				//{
				//	SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
				//}
			}
#ifdef _WIN32
			SDL_StopTextInput(gWindow.getWindow());
#elif __linux__
			SDL_StopTextInput();
#endif
		}
		close();
	}

	delete myTimer;
	delete clock;
	return 0;
}
