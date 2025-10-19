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
#include <cmath>

#include "SoftfootFalls.h"

#include "Print.h"
#include "Time.h"
#include "Clock.h"
#include "Timer.h"

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
#include "SDLInterface.h"
#include "SDLWrapper.h"

#include "Load.h"
#include "Container.h"


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
	void Free();
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);
#ifdef _WIN32
	void render(int x, int y, SDL_FRect* clip = NULL, double angle = 0.0, SDL_FPoint* center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE, Distance distance = {0, 0});
#elif __linux__
	void render(int x, int y, SDL_Rect* clip = NULL, double secondAngle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, Distance distance = { 0, 0 });
#endif
	void setAsRenderTarget();
	int GetWidth();
	int GetHeight();
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

class LButton
{
public:
	LButton();
	void setPosition(int x, int y);
	void HandleEvent(SDL_Event* e);
	void render();
private:
	SDL_Point mPosition;
	LButtonSprite mCurrentSprite;
};

class Dot
{
public:
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	static const int DOT_VEL = 100;
	Dot();
	~Dot();
	void HandleEvent(SDL_Event& e);
	void move(double timestepX, double timestepY);
	void setIJ(int*, int*, float*, float*);
	void render();
private:
	SDL_Rect mBox;
	int mVelX, mVelY;
};



class LBitmapFont
{
public:
	LBitmapFont();
	bool buildFont(std::string path);
	void Free();
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

bool Init();
bool loadMedia();
void close();
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
static int current_sine_sample = 0;
LTexture gDotTexture;
LTexture gBGTexture;
const int HORIZON_SIZE = 8;
LTexture gHorizons[HORIZON_SIZE];
LTexture gBoxes[KEY_PRESS_SURFACE_TOTAL];
LTexture* gBox = &gBoxes[KEY_PRESS_SURFACE_UP];
LTexture gBoxFront;
LTexture gPlayerHighlight;
LTexture characterFairyHopeful;
Sint32 gData[TOTAL_DATA];
LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gBlueTexture;
LTexture gShimmerTexture;
LBitmapFont gBitmapFont;
LTexture gTargetTexture;
Directions gDirection = DIRECTION_UP;
const int ROW_SIZE = 15;


LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mSurfacePixels = NULL;
}

LTexture::~LTexture()
{
	Free();
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
	Free();
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
	Free();
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

void LTexture::Free()
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
void LTexture::render(int x, int y, SDL_FRect* clip, double angle, SDL_FPoint* center, SDL_FlipMode flip, Distance distance)
#elif __linux__
void LTexture::render(int x, int y, SDL_Rect* clip, double secondAngle, SDL_Point* center, SDL_RendererFlip flip, Distance distance)
#endif
{
	float width = 0.0f;
	float height = 0.0f;
	if (distance.width == 0.0f && distance.height == 0.0f)
	{
		width = mWidth;
		height = mHeight;
	}
	else
	{
		width = distance.width;
		height = distance.height;
	}
#ifdef _WIN32
	SDL_FRect renderQuad = { (float)x, (float)y, width, height };
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

int LTexture::GetWidth()
{
	return mWidth;
}

int LTexture::GetHeight()
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

SDL_Texture* LTexture::getTexture()
{
	return mTexture;
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

void LButton::HandleEvent(SDL_Event* e)
{
#ifdef _WIN32
	if (e->type == SDL_EVENT_MOUSE_MOTION || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_UP)
#elif __linix__
	if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP)
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

Dot::Dot()
{
	mBox.x = 0;
	mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;
	mVelX = 0;
	mVelY = 0;
}

Dot::~Dot()
{
}

void Dot::HandleEvent(SDL_Event& e)
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
		case SDLK_UP:
			mVelY -= DOT_VEL;
			break;
		case SDLK_DOWN:
			mVelY += DOT_VEL;
			break;
		case SDLK_LEFT:
			mVelX -= DOT_VEL;
			break;
		case SDLK_RIGHT:
			mVelX += DOT_VEL;
			break;
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
		case SDLK_UP:
			mVelY += DOT_VEL;
			break;
		case SDLK_DOWN:
			mVelY -= DOT_VEL;
			break;
		case SDLK_LEFT:
			mVelX += DOT_VEL;
			break;
		case SDLK_RIGHT:
			mVelX -= DOT_VEL;
			break;
		}
	}
}

void Dot::move(double timeStepX, double timeStepY)
{
	double potentialX = mBox.x + (mVelX * timeStepX);
	if (potentialX < 0)
	{
		mBox.x = 0;
	}
	else if (potentialX > gWindow.GetWidth() - (mBox.w * 4.0))
	{
		mBox.x = gWindow.GetWidth() - (mBox.w * 4);
	}
	else
	{
		mBox.x = potentialX;
	}
	double potentialY = mBox.y + (mVelY * timeStepY);
	if (potentialY < 0)
	{
		mBox.y = 0;
	}
	else if (potentialY > gWindow.GetHeight() - (mBox.h * 4.0))
	{
		mBox.y = gWindow.GetHeight() - (mBox.h * 4.0);
	}
	else
	{
		mBox.y = potentialY;
	}
}

void Dot::setIJ(int* i, int* j, float* normalI, float* normalJ)
{
	float width = (float)gWindow.GetWidth();
	float height = (float)gWindow.GetHeight();
	float x = (float)mBox.x;
	float y = (float)mBox.y;
	float jNormal = x / width;
	float iNormal = y / height;
	*i = ROW_SIZE - (ROW_SIZE * iNormal);
	if (*i == ROW_SIZE) *i = ROW_SIZE - 1;
	*j = ROW_SIZE * jNormal;
	*normalI = iNormal;
	*normalJ = jNormal;
}

void Dot::render()
{
	Distance distance = { mBox.w * 5.0f, mBox.h * 5.0f };
	//gDotTexture.render(mBox.x - (distance.width / 2), mBox.y + (distance.height / 2));
	gBoxFront.render(mBox.x, mBox.y, NULL, 0.0, NULL, SDL_FLIP_NONE, distance);
	//renderParticles();
}

LWindow::LWindow()
{
	window = NULL;
	renderer = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	width = 0;
	height = 0;
}

bool LWindow::Init(bool bCreateRenderer)
{
#ifdef _WIN32
	if (bCreateRenderer)
	{
		if (!SDL_CreateWindowAndRenderer("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer))
		{
			return false;
		}
		//SDL_SetRenderVSync(renderer, 1);
		SDL_SetRenderVSync(gRenderer, SDL_RENDERER_VSYNC_DISABLED);
	}
	else
	{
		window = SDL_CreateWindow("SDL_Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
		if (window == NULL)
		{
			return false;
		}
	}

	mMouseFocus = true;
	mKeyboardFocus = true;
	width = SCREEN_WIDTH;
	height = SCREEN_HEIGHT;
	return true;
#elif __linux__
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window != NULL)
	{
		mMouseFocus = true;
		mKeyboardFocus = true;
		width = SCREEN_WIDTH;
		height = SCREEN_HEIGHT;
	}
	return window != NULL;
#endif
}

#ifdef __linux__
SDL_Renderer* LWindow::createRenderer()
{
	return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
}
#endif

SDL_Renderer* LWindow::GetRenderer()
{
	return renderer;
}

void LWindow::HandleEvent(SDL_Event& event)
{
	bool updateCaption = false;

	switch (event.type)
	{
#ifdef _WIN32
	case SDL_EVENT_WINDOW_RESIZED:
#elif __linux__
	case SDL_WINDOWEVENT_SIZE_CHANGED:
#endif
		width = event.window.data1;
		height = event.window.data2;
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
		if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_RETURN)
#elif __linux__
		if (event.key.keysym.sym == SDLK_RETURN)
#endif
		{
			if (mFullScreen)
			{
#ifdef _WIN32
				SDL_SetWindowFullscreen(window, false);
#elif __linux__
				SDL_SetWindowFullscreen(window, 0);
#endif
				mFullScreen = false;
			}
			else
			{
#ifdef _WIN32
				SDL_SetWindowFullscreen(window, true);
#elif __linux__
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
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
		SDL_SetWindowTitle(window, caption.str().c_str());
	}
}

void LWindow::Free()
{
	if (window != NULL)
	{
		SDL_DestroyWindow(window);
	}
	mMouseFocus = false;
	mKeyboardFocus = false;
	width = 0;
	height = 0;
}

int LWindow::GetWidth()
{
	return width;
}

int LWindow::GetHeight()
{
	return height;
}

SDL_Window* LWindow::GetWindow()
{
	return window;
}

bool LWindow::HasMouseFocus()
{
	return mMouseFocus;
}

bool LWindow::HasKeyboardFocus()
{
	return mKeyboardFocus;
}

bool LWindow::IsMinimized()
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
	Free();
	bool success = true;
	if (!mFontTexture.loadPixelsFromFile(path.c_str()))
	{
		SDL_Log("Unable to load bitmap font surface!\n");
		success = false;
	}
	else
	{
		Uint32 bgColor = mFontTexture.getPixel32(0, 0);
		int cellW = mFontTexture.GetWidth() / 16;
		int cellH = mFontTexture.GetHeight() / 16;
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

void LBitmapFont::Free()
{
	mFontTexture.Free();
}

void LBitmapFont::renderText(int x, int y, std::string text)
{
	if (mFontTexture.GetWidth() > 0)
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

bool Init()
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
		if (!gWindow.Init(true))
		{
			SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
#ifdef _WIN32
			gRenderer = gWindow.GetRenderer();
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
			//gScreenSurface = SDL_GetWindowSurface(gWindow);
			gScreenSurface = SDL_GetWindowSurface(gWindow.GetWindow());
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
	if (!(success = gModulatedTexture.loadFromFile(load->Path("Landscape_Moon_3300x2550.png")))) {}
	else
	{
		gModulatedTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	}
	if (!(success = gWalkingSpriteSheetTexture.loadFromFile(load->Path("Player_1024x1024.png")))) {}
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
	success = gIconCursor.loadFromFile(load->Path("IconCursor.png"));
	success = gSun.loadFromFile(load->Path("CharacterFairySun_000_256x256.png"));
	success = gMoon.loadFromFile(load->Path("CharacterFairyMoon_000_256x256.png"));
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
	if (!(success = gButtonSpriteSheetTexture.loadFromFile(load->Path("button.png")))) {}
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
		gButtons[1].setPosition(gWindow.GetWidth() - BUTTON_WIDTH, 0);
		gButtons[2].setPosition(0, gWindow.GetHeight() - BUTTON_HEIGHT);
		gButtons[3].setPosition(gWindow.GetWidth() - BUTTON_WIDTH, gWindow.GetHeight() - BUTTON_HEIGHT);
	}

	success = gDotTexture.loadFromFile(load->Path("CharacterFairySmallwig000_64x64.png"));
	success = gBGTexture.loadFromFile(load->Path("bg.png"));
	success = gHorizons[0].loadFromFile(load->Path("Horizon000.png"));
	success = gHorizons[1].loadFromFile(load->Path("Horizon001.png"));
	success = gHorizons[2].loadFromFile(load->Path("Horizon002.png"));
	success = gHorizons[3].loadFromFile(load->Path("Horizon003.png"));
	success = gHorizons[4].loadFromFile(load->Path("Horizon004.png"));
	success = gHorizons[5].loadFromFile(load->Path("Horizon005.png"));
	success = gHorizons[6].loadFromFile(load->Path("Horizon006.png"));
	success = gHorizons[7].loadFromFile(load->Path("Horizon007.png"));
	success = gBoxes[KEY_PRESS_SURFACE_UP].loadFromFile(load->Path("BoxUp.png"));
	success = gBoxes[KEY_PRESS_SURFACE_DOWN].loadFromFile(load->Path("BoxDown.png"));
	success = gBoxes[KEY_PRESS_SURFACE_LEFT].loadFromFile(load->Path("BoxLeft.png"));
	success = gBoxes[KEY_PRESS_SURFACE_RIGHT].loadFromFile(load->Path("BoxRight.png"));
	success = gBoxFront.loadFromFile(load->Path("BoxFront.png"));
	success = gPlayerHighlight.loadFromFile(load->Path("PlayerHighlight_000_1024x1024.png"));
	success = characterFairyHopeful.loadFromFile(load->Path("CharacterFairyHopeful_000_256x256.png"));
	success = gRedTexture.loadFromFile(load->Path("CharacterFairySmallcol000_64x64.png"));
	success = gGreenTexture.loadFromFile(load->Path("CharacterFairySmalling000_64x64.png"));
	success = gBlueTexture.loadFromFile(load->Path("CharacterFairySmallbin000_64x64.png"));
	success = gShimmerTexture.loadFromFile(load->Path("CharacterFairySmallton000_64x64.png"));
	//mSurfacePixels = IMG_Load(path);
	gRedTexture.setAlpha(192);
	gGreenTexture.setAlpha(192);
	gBlueTexture.setAlpha(192);
	gShimmerTexture.setAlpha(192);
	if (!gBitmapFont.buildFont(load->Path("font_000.png")))
	{
		SDL_Log("Failed to load bitmap font!\n");
		success = false;
	}
	if (!gTargetTexture.createBlank(gWindow.GetWidth(), gWindow.GetHeight(), SDL_TEXTUREACCESS_TARGET))
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
	gModulatedTexture.Free();
	gWalkingSpriteSheetTexture.Free();
	gIconCursor.Free();
	gSun.Free();
	gMoon.Free();
	gButtonSpriteSheetTexture.Free();
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
	gDotTexture.Free();
	gBGTexture.Free();
	for (int i = 0; i < HORIZON_SIZE; ++i)
	{
		gHorizons[i].Free();
	}
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
	{
		gBoxes[i].Free();
	}
	gBoxFront.Free();
	gPlayerHighlight.Free();
	characterFairyHopeful.Free();
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
	gRedTexture.Free();
	gGreenTexture.Free();
	gBlueTexture.Free();
	gShimmerTexture.Free();
	gBitmapFont.Free();
	gTargetTexture.Free();

	delete load;
	SDL_DestroyRenderer(gRenderer);
	gWindow.Free();
	SDL_Quit();
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
	Load* load = new Load(SDL_GetBasePath());
	const int GRID_SIZE = ROW_SIZE * ROW_SIZE;

	if (!Init())
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
			int countedFrames = 0;
			Dot dot;
			int backgroundScrollingOffset = 0;
			int horizonScrollingOffset = 0;
			std::string inputText = "Input";
			int currentData = 0;
			SDL_Color highlightColor = { 0xFF, 0, 0, 0xFF };
			double minuteAngle = 0;
			double hourAngle = 0;
			double halfDayAngle = 0;
			double fullDayAngle = 0;
#ifdef _WIN32
			SDL_FPoint screenCenter = { gWindow.GetWidth() / 2, gWindow.GetHeight() / 2 };
#elif __linux__
			SDL_Point screenCenter = { gWindow.GetWidth() / 2, gWindow.GetHeight() / 2 };
#endif
			bool isDebug = false;
			bool isInput = false;
#ifdef _WIN32
			float mouseX, mouseY;
#elif __linux__
			int mouseX, mouseY;
#endif

			long long previousTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			long long decisecondDelta = 0L;
			long long secondDelta = 0L;
			long long trisecondDelta = 0L;
			long long minuteDelta = 0L;
			long long hourDelta = 0L;
			long long halfDayDelta = 0L;
			long long fullDayDelta = 0L;
			long long playerDelta = 0L;
			bool trisecondToggle = false;
			bool playerToggle = false;
			int gridCounter = 0;
			int playerI = 0;
			int playerJ = 0;
			int dotI = 0;
			int dotJ = 0;
			float dotNormalI = 0.0f;
			float dotNormalJ = 0.0f;
			while (!quit)
			{
				long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				decisecondDelta += currentTime - previousTime;
				secondDelta += currentTime - previousTime;
				playerDelta += currentTime - previousTime;
				trisecondDelta += currentTime - previousTime;
				minuteDelta += currentTime - previousTime;
				hourDelta += currentTime - previousTime;
				halfDayDelta += currentTime - previousTime;
				fullDayDelta += currentTime - previousTime;
				double decisecondLimit = 100'000'000;
				double secondLimit = 1'000'000'000;
				double trisecondLimit = 3'000'000'000;
				double minuteLimit = 60'000'000'000;
				double hourLimit = 3'600'000'000'000;
				double halfDayLimit = 43'200'000'000'000;
				double fullDayLimit = 86'400'000'000'000;
				double playerLimit = secondLimit;
				if (decisecondDelta > decisecondLimit)
				{
					decisecondDelta -= decisecondLimit;
					++gridCounter;
					if (gridCounter > ROW_SIZE * ROW_SIZE)
					{
						gridCounter = 0;
					}
				}
				if (secondDelta > secondLimit)
				{
					secondDelta -= secondLimit;
				}
				if (trisecondDelta > trisecondLimit)
				{
					trisecondToggle = !trisecondToggle;
					trisecondDelta -= trisecondLimit;
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
				double decisecondNormal = (double)decisecondDelta / decisecondLimit;
				double secondNormal = (double)secondDelta / secondLimit;
				double trisecondNormal = (double)trisecondDelta / trisecondLimit;
				double minuteNormal = (double)minuteDelta / minuteLimit;
				double hourNormal = (double)hourDelta / hourLimit;
				double halfDayNormal = (double)halfDayDelta / halfDayLimit;
				double fullDayNormal = (double)fullDayDelta / fullDayLimit;
				double playerNormal = (double)playerDelta / secondLimit;
				previousTime = currentTime;

				SDL_GetMouseState(&mouseX, &mouseY);
				myTimer->Update();
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
						//if (event.jaxis.which == 0)
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
					else if (e.type == KEY_RELEASED)
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
					else if (e.type == KEY_PRESSED)
					{
#ifdef _WIN32
						switch (e.key.key)
#elif __linux__
						switch (e.key.keysym.sym)
#endif
						{
						case SDLK_HOME:
							isDebug = !isDebug;
							if (isInput) StopTextInput(gWindow.GetWindow());
							else StartTextInput(gWindow.GetWindow());
							isInput = !isInput;
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
#ifdef _WIN32
							PlaySound(L"C:/Users/belenbaa/Desktop/SoftfootFalls/SoftfootFalls/x64/Debug/art/scratch.wav", NULL, SND_FILENAME | SND_ASYNC);
#elif __linux__
							system("aplay ~/SoftfootFalls/SoftfootFalls/x64/Debug/art/scratch.wav");
#endif
							break;
						case SDLK_UP:
						case KEY_W:
							gBox = &gBoxes[KEY_PRESS_SURFACE_UP];
							--currentData;
							if (currentData < 0)
							{
								currentData = TOTAL_DATA - 1;
							}
							++playerI;
							if (playerI >= ROW_SIZE) playerI = ROW_SIZE - 1;
							playerToggle = true;
							break;
						case SDLK_DOWN:
						case KEY_S:
							gBox = &gBoxes[KEY_PRESS_SURFACE_DOWN];
							++currentData;
							if (currentData == TOTAL_DATA)
							{
								currentData = 0;
							}
							--playerI;
							if (playerI < 0) playerI = 0;
							playerToggle = true;
							break;
						case SDLK_LEFT:
						case KEY_A:
							gBox = &gBoxes[KEY_PRESS_SURFACE_LEFT];
							--gData[currentData];
							--playerJ;
							if (playerJ < 0) playerJ = 0;
							playerToggle = true;
							degrees -= 60;
							break;
						case SDLK_RIGHT:
						case KEY_D:
							gBox = &gBoxes[KEY_PRESS_SURFACE_RIGHT];
							++gData[currentData];
							++playerJ;
							if (playerJ >= ROW_SIZE) playerJ = ROW_SIZE - 1;
							playerToggle = true;
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
						gButtons[i].HandleEvent(&e);
					}
					dot.HandleEvent(e);
					gWindow.HandleEvent(e);
				}
				if (!gWindow.IsMinimized())
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
						backgroundScrollingOffset = -gBGTexture.GetHeight() * minuteNormal;
						if (backgroundScrollingOffset < -gBGTexture.GetHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case DIRECTION_DOWN:
						backgroundScrollingOffset = gBGTexture.GetHeight() * minuteNormal;
						if (backgroundScrollingOffset > gBGTexture.GetHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case DIRECTION_LEFT:
						backgroundScrollingOffset = -gBGTexture.GetWidth() * minuteNormal;
						if (backgroundScrollingOffset < -gBGTexture.GetWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case DIRECTION_RIGHT:
						backgroundScrollingOffset = gBGTexture.GetWidth() * minuteNormal;
						if (backgroundScrollingOffset > gBGTexture.GetWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					}

					minuteAngle = 360 * minuteNormal;
					hourAngle = 360 * hourNormal;
					halfDayAngle = 360 * halfDayNormal;
					fullDayAngle = 360 * fullDayNormal;

#ifdef _WIN32
					SDL_FRect fillRect = { gWindow.GetWidth() / 4, gWindow.GetHeight() / 4, gWindow.GetWidth() / 2, gWindow.GetHeight() / 2 };
#elif __linux__
					SDL_Rect fillRect = { gWindow.GetWidth() / 4, gWindow.GetHeight() / 4, gWindow.GetWidth() / 2, gWindow.GetHeight() / 2 };
#endif
#ifdef _WIN32
					SDL_FRect outlineRect = { gWindow.GetWidth() / 6, gWindow.GetHeight() / 6, gWindow.GetWidth() * 2 / 3, gWindow.GetHeight() * 2 / 3 };
#elif __linux__
					SDL_Rect outlineRect = { gWindow.GetWidth() / 6, gWindow.GetHeight() / 6, gWindow.GetWidth() * 2 / 3, gWindow.GetHeight() * 2 / 3 };
#endif

					switch (gDirection)
					{
					case DIRECTION_UP:
						gBGTexture.render(0, backgroundScrollingOffset);
						gBGTexture.render(0, backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.render(0, backgroundScrollingOffset + (gBGTexture.GetHeight() * 2));
						gBGTexture.render(gBGTexture.GetWidth(), backgroundScrollingOffset);
						gBGTexture.render(gBGTexture.GetWidth(), backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.render(gBGTexture.GetWidth(), backgroundScrollingOffset + (gBGTexture.GetHeight() * 2));
						break;
					case DIRECTION_DOWN:
						gBGTexture.render(0, backgroundScrollingOffset);
						gBGTexture.render(0, backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.render(0, backgroundScrollingOffset - gBGTexture.GetHeight());
						gBGTexture.render(gBGTexture.GetWidth(), backgroundScrollingOffset);
						gBGTexture.render(gBGTexture.GetWidth(), backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.render(gBGTexture.GetWidth(), backgroundScrollingOffset - gBGTexture.GetHeight());
						break;
					case DIRECTION_LEFT:
						gBGTexture.render(backgroundScrollingOffset, 0);
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.GetWidth(), 0);
						gBGTexture.render(backgroundScrollingOffset + (gBGTexture.GetWidth() * 2), 0);
						gBGTexture.render(backgroundScrollingOffset, gBGTexture.GetHeight());
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.GetWidth(), gBGTexture.GetHeight());
						gBGTexture.render(backgroundScrollingOffset + (gBGTexture.GetWidth() * 2), gBGTexture.GetHeight());
						break;
					case DIRECTION_RIGHT:
						gBGTexture.render(backgroundScrollingOffset, 0);
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.GetWidth(), 0);
						gBGTexture.render(backgroundScrollingOffset - gBGTexture.GetWidth(), 0);
						gBGTexture.render(backgroundScrollingOffset, gBGTexture.GetHeight());
						gBGTexture.render(backgroundScrollingOffset + gBGTexture.GetWidth(), gBGTexture.GetHeight());
						gBGTexture.render(backgroundScrollingOffset - gBGTexture.GetWidth(), gBGTexture.GetHeight());
						break;
					}
					SDL_Rect fullscreenViewport;
					fullscreenViewport.x = 0;
					fullscreenViewport.y = 0;
					fullscreenViewport.w = gWindow.GetWidth();
					fullscreenViewport.h = gWindow.GetHeight();
					if (isDebug)
					{
						SetRenderViewport(gRenderer, &fullscreenViewport);
						gModulatedTexture.render(0, 0);
						RenderTexture(gRenderer, gModulatedTexture.getTexture());
					}
					dot.move((double)gWindow.GetWidth() * 0.000025, (double)gWindow.GetHeight() * 0.000025);
					dot.setIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ);

					SDL_Rect middleViewport;
					middleViewport.x = (gWindow.GetWidth() * 0.5f) - 100;
					middleViewport.y = (gWindow.GetHeight() * 0.5f) - 100;
					middleViewport.w = 200;
					middleViewport.h = 200;
					SetRenderViewport(gRenderer, NULL);
					float normal = secondNormal;
					float flippyNormal = trisecondNormal;
					if (trisecondToggle)
					{
						flippyNormal = 1.0f - trisecondNormal;
					}
					float centerX = mouseX;
					if (centerX > gWindow.GetWidth() * 0.15f) centerX = gWindow.GetWidth() * 0.15f;
					float centerY = gWindow.GetHeight() - mouseY;
					if (centerY > gWindow.GetHeight() * 0.5f) centerY = gWindow.GetHeight() * 0.5f;
					int horizonI = 0;
					SDL_Rect defaultRect = { -1, -1, -1, -1 };
					for (int i = ROW_SIZE - 1; i >= 0 ; --i)
					{
						normal = (float)i / (float)ROW_SIZE;
						float x = (0 + centerX) * normal;
						middleViewport.w = centerX / ROW_SIZE;
						middleViewport.h = centerY / ROW_SIZE;
						float y = 
							gWindow.GetHeight()
							- middleViewport.h
							- ((0 + centerY) * normal);
						middleViewport.x = x;
						middleViewport.y = y;
						SDL_Rect leftViewport = middleViewport;
						x = gWindow.GetWidth() - middleViewport.w - ((0 + centerX) * normal);
						middleViewport.w = centerX / ROW_SIZE;
						middleViewport.h = centerY / ROW_SIZE;
						y = 
							gWindow.GetHeight()
							- middleViewport.h
							- ((0 + centerY) * normal);
						middleViewport.x = x;
						middleViewport.y = y;
						SDL_Rect rightViewport = middleViewport;
						SDL_Rect topLeft = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
						SDL_Rect farLeft = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
						SDL_Rect farRight = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
						for (int j = 0; j < ROW_SIZE; ++j)
						{
							float width = rightViewport.x + rightViewport.w - leftViewport.x;
							float xStep = width / ROW_SIZE;
							x = leftViewport.x + (j * xStep);
							middleViewport.w = width / ROW_SIZE;
							middleViewport.h = middleViewport.w;
							y = 
								gWindow.GetHeight()
								- middleViewport.h
								- ((0 + centerY) * normal);
							middleViewport.x = x;
							middleViewport.y = y;
							if (middleViewport.x < 0) middleViewport.x = 0;
							if (middleViewport.y < 0) middleViewport.y = 0;
							if (middleViewport.w < 0) middleViewport.w = 0;
							if (middleViewport.h < 0) middleViewport.h = 0;
							++middleViewport.w;
							SetRenderViewport(gRenderer, &middleViewport);
							int modI = (i % 2 == 0) ? 0 : 4;
							RenderTexture(gRenderer, gHorizons[modI + horizonI].getTexture());
							++horizonI;
							if (horizonI > 3)
							{
								horizonI = 0;
							}
							if (dotI == i && dotJ == j)
							{
								RenderTexture(gRenderer, gPlayerHighlight.getTexture());
							}
							Distance distance = { middleViewport.w, middleViewport.h };
							if (i == 0 && j == 0)
							{
								gIconCursor.render(
									0,
									0,
									NULL,
									joystickAngle,
									NULL,
									flipType,
									distance);
							}
							if (i == 0 && j == 0)
							{
								topLeft.x = middleViewport.x;
								topLeft.y = middleViewport.y;
								topLeft.w = middleViewport.w;
								topLeft.h = middleViewport.h;
							}
							if (j == 0)
							{
								farLeft.x = middleViewport.x;
								farLeft.y = middleViewport.y;
								farLeft.w = middleViewport.w;
								farLeft.h = middleViewport.h;
							}
							if (i == 0 && j == ROW_SIZE - 1)
							{
								farRight.x = middleViewport.x;
								farRight.y = middleViewport.y;
								farRight.w = middleViewport.w;
								farRight.h = middleViewport.h;
							}
							if (i == 0 && j == ROW_SIZE - 1)
							{
								gIconCursor.render(
									0,
									0,
									NULL,
									degrees,
									NULL,
									flipType,
									distance);
							}
							if (i == ROW_SIZE - 1 && j == 0)
							{
								gSun.render(
									0,
									0,
									NULL,
									-minuteAngle,
									NULL,
									SDL_FLIP_NONE,
									distance);
							}
							if (i == ROW_SIZE - 1 && j == ROW_SIZE - 1)
							{
								gMoon.render(
									0,
									0,
									NULL,
									minuteAngle,
									NULL,
									SDL_FLIP_NONE,
									distance);
							}
							if (i == ROW_SIZE - 1 && j == 0)
							{
								SetRenderViewport(gRenderer, NULL);
								int x = 0;
								int y = middleViewport.y - (middleViewport.h * 2.0f);
								if (y < 0)
								{
									y = 0;
								}
								if (inputText != "")
								{
									gBitmapFont.renderText(x, y, inputText.c_str());
								}
								else
								{
									gBitmapFont.renderText(x, y, "");
								}
								//gBitmapFont.renderText(0, 0, std::to_string(gData[currentData]).c_str());
								SetRenderViewport(gRenderer, &middleViewport);
							}
							if (playerI == i && playerJ == j)
							{
								//RenderTexture(gRenderer, gTexture);
								//RenderTexture(gRenderer, gBox->getTexture());
							}

							SDL_Rect walkingSpriteViewport;
							walkingSpriteViewport.x = farLeft.x + ((farLeft.w * ROW_SIZE) * dotNormalJ) - (farLeft.w / 3);
							walkingSpriteViewport.y = farLeft.y - (farLeft.h * 0.5f);
							walkingSpriteViewport.w = farLeft.w / 2;
							walkingSpriteViewport.h = farLeft.h / 2;
							if (dotI == i)
							{
								SetRenderViewport(gRenderer, &walkingSpriteViewport);
								RenderTexture(gRenderer, gBox->getTexture());
								SetRenderViewport(gRenderer, &walkingSpriteViewport);
							}
							if (dotI == i && dotJ == j)
							{

								SDL_Rect circleViewport;
								float xOffset = (walkingSpriteViewport.w * trisecondNormal);
								if (trisecondToggle) xOffset = walkingSpriteViewport.w * (1.0f - trisecondNormal);
								circleViewport.w = walkingSpriteViewport.w;
								circleViewport.h = walkingSpriteViewport.h;
								circleViewport.x = walkingSpriteViewport.x - (circleViewport.w / 2) + xOffset;
								circleViewport.y = walkingSpriteViewport.y;

								double radians = (360.0 * trisecondNormal) * (M_PI / 180.0);
								double circleX = cos(radians) * (walkingSpriteViewport.w / 2);
								double circleY = sin(radians) * (walkingSpriteViewport.h / 2);
								if (trisecondToggle)
								{
									circleY = circleY * trisecondNormal;
								}
								else
								{
									circleY = circleY * (1.0f - trisecondNormal);
								}
								SDL_Rect radianViewport;
								radianViewport.x = walkingSpriteViewport.x + (walkingSpriteViewport.w / 2) + circleX;
								radianViewport.y = walkingSpriteViewport.y + (walkingSpriteViewport.h / 2) + circleY;
								if (trisecondToggle)
								{
									radianViewport.w = 100 * trisecondNormal;
									radianViewport.h = 100 * trisecondNormal;
								}
								else
								{
									radianViewport.w = 100 * (1.0f - trisecondNormal);
									radianViewport.h = 100 * (1.0f - trisecondNormal);
								}

								if (!trisecondToggle)
								{
									SetRenderViewport(gRenderer, &radianViewport);
									RenderTexture(gRenderer, characterFairyHopeful.getTexture());
								}
								SetRenderViewport(gRenderer, &middleViewport);
								SDL_Rect bottomViewport = middleViewport;
								bottomViewport.y += bottomViewport.h / 2;
								SetRenderViewport(gRenderer, &bottomViewport);
								if (trisecondToggle)
								{
									SetRenderViewport(gRenderer, &radianViewport);
									RenderTexture(gRenderer, characterFairyHopeful.getTexture());
								}
							}
						}
					}

					SetRenderViewport(gRenderer, &fullscreenViewport);
					dot.render();
					SetRenderViewport(gRenderer, NULL);

					if (isDebug)
					{
						for (int i = 0; i < TOTAL_BUTTONS; ++i)
						{
							gButtons[i].render();
						}
						gTargetTexture.setAsRenderTarget();
						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x00);
						SDL_RenderClear(gRenderer);
						fillRect = { (float)gWindow.GetWidth() / 4, (float)gWindow.GetHeight() / 4, (float)gWindow.GetWidth() / 2, (float)gWindow.GetHeight() / 2 };
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
						SDL_RenderFillRect(gRenderer, &fillRect);
						outlineRect = { (float)gWindow.GetWidth() / 6, (float)gWindow.GetHeight() / 6, (float)gWindow.GetWidth() * 2 / 3, (float)gWindow.GetHeight() * 2 / 3 };
						SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
						RenderRect(gRenderer, &outlineRect);
						SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
						RenderLine(gRenderer, 0, gWindow.GetHeight() / 2, gWindow.GetWidth(), gWindow.GetHeight() / 2);
						SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
						for (int i = 0; i < gWindow.GetHeight(); i += 4)
						{
							RenderPoint(gRenderer, gWindow.GetWidth() / 2, i);
						}
						SDL_SetRenderTarget(gRenderer, NULL);
						gTargetTexture.render(-gWindow.GetWidth() * 0.25f, -gWindow.GetHeight() * 0.25f, NULL, fullDayAngle, &screenCenter);
						gTargetTexture.render(-gWindow.GetWidth() * 0.25f, -gWindow.GetHeight() * 0.25f, NULL, halfDayAngle, &screenCenter);
						gTargetTexture.render(-gWindow.GetWidth() * 0.25f, -gWindow.GetHeight() * 0.25f, NULL, hourAngle, &screenCenter);
						gTargetTexture.render(-gWindow.GetWidth() * 0.25f, -gWindow.GetHeight() * 0.25f, NULL, minuteAngle, &screenCenter);
						gBitmapFont.renderText(0, 0, std::to_string(gData[currentData]).c_str());
						RenderLine(gRenderer, mouseX, mouseY, 0, 0);
						RenderLine(gRenderer, mouseX, mouseY, gWindow.GetWidth(), 0);
						RenderLine(gRenderer, mouseX, mouseY, 0, gWindow.GetHeight());
						RenderLine(gRenderer, mouseX, mouseY, gWindow.GetWidth(), gWindow.GetHeight());
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
		}
		close();
	}

	delete load;
	delete myTimer;
	delete clock;
	return 0;
}
