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


const char* BasePath(const char* filePath)
{
	char* basePath = new char[strlen(filePath) + 1];
	int i = 0;
	while (i < strlen(filePath)) basePath[i++] = filePath[i];
	--i;
	do basePath[i] = '\0';
	while (--i, filePath[i] != SEPARATOR);
	return basePath;
}

const int JOYSTICK_DEAD_ZONE = 8000;
#ifdef _WIN32
const double M_PI = 3.14159265359;
#endif
const int TOTAL_DATA = 10;

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
	void Free();
	void setBlendMode(SDL_BlendMode blending);
#ifdef _WIN32
	void render(int x, int y, SDL_FRect* clip = NULL, double angle = 0.0, SDL_FPoint* center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE, Distance distance = {0, 0});
#elif __linux__
	void render(int x, int y, SDL_Rect* clip = NULL, double secondAngle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, Distance distance = { 0, 0 });
#endif
	int GetWidth() const;
	int GetHeight() const;
	Uint32 getPixel32(Uint32 x, Uint32 y);
	Uint32 getPitch32();
	SDL_Texture* getTexture();
private:
	SDL_Texture* mTexture;
	SDL_Surface* mSurfacePixels;
	void* mRawPixels;
	int mRawPitch;
	int mWidth;
	int mHeight;
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
	void setIJ(int*, int*, float*, float*) const;
	void render() const;
	SDL_Rect getBox();
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
bool loadMedia(Load*);
void close(Load*);
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
SDL_Gamepad* gGameController;
#elif __linux__
SDL_GameController* gGameController;
#endif
SDL_Joystick* gJoystick = NULL;
SDL_Haptic* gJoyHaptic = NULL;
LTexture gDotTexture;
LTexture gBGTexture;
const int HORIZON_SIZE = 8;
LTexture gHorizons[HORIZON_SIZE];
LTexture gBoxes[KEY_PRESS_SURFACE_TOTAL];
LTexture* gBox = &gBoxes[KEY_PRESS_SURFACE_UP];
LTexture gBoxFront;
LTexture gPlayerHighlight;
LTexture gPlayerBeam;
LTexture characterFairyHopeful;
Sint32 gData[TOTAL_DATA];
LBitmapFont gBitmapFont;
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
		DestroySurface(mSurfacePixels);
		mSurfacePixels = NULL;
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
		DestroySurface(mSurfacePixels);
		mSurfacePixels = NULL;
	}
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending);
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

int LTexture::GetWidth() const
{
	return mWidth;
}

int LTexture::GetHeight() const
{
	return mHeight;
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

SDL_Texture* LTexture::getTexture()
{
	return mTexture;
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
	if (e.type == KEY_PRESSED && e.key.repeat == 0)
	{
		switch (Key(e))
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
	else if (e.type == KEY_RELEASED && e.key.repeat == 0)
	{
		switch (Key(e))
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

void Dot::setIJ(int* i, int* j, float* normalI, float* normalJ) const
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

void Dot::render() const
{
	Distance distance = { mBox.w * 5.0f, mBox.h * 5.0f };
	gBoxFront.render(mBox.x, mBox.y, NULL, 0.0, NULL, SDL_FLIP_NONE, distance);
}

SDL_Rect Dot::getBox()
{
	SDL_Rect rect =
	{
		mBox.x,
		mBox.y,
		mBox.w,
		mBox.h
	};
	return rect;
}

bool LWindow::Init()
{
	const char* title = "SoftfootFalls";
#ifdef _WIN32
	if (!SDL_CreateWindowAndRenderer(title, gWindow.GetWidth(), gWindow.GetHeight(), SDL_WINDOW_RESIZABLE, &window, &renderer))
	{
		return false;
	}
	SDL_SetRenderVSync(renderer, 1);
	//SDL_SetRenderVSync(gRenderer, SDL_RENDERER_VSYNC_DISABLED);

	width = gWindow.GetWidth();
	height = gWindow.GetHeight();
	return true;
#elif __linux__
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gWindow.GetWidth(), gWindow.GetHeight(), SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window != NULL)
	{
		width = gWindow.GetWidth();
		height = gWindow.GetHeight();
	}
	return window != NULL;
#endif
}

#ifdef __linux__
SDL_Renderer* LWindow::CreateRenderer()
{
	return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
}
#endif

SDL_Renderer* LWindow::GetRenderer()
{
	return renderer;
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
		for (char i = 0; i < text.length(); ++i)
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
		if (!gWindow.Init())
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
			gRenderer = gWindow.CreateRenderer();
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
			gScreenSurface = SDL_GetWindowSurface(gWindow.GetWindow());
		}
	}

	return success;
}

bool loadMedia(Load* load)
{
	using namespace std;

	bool success = true;

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
	success = gPlayerBeam.loadFromFile(load->Path("PlayerBeam_000_2048x2048.png"));
	success = characterFairyHopeful.loadFromFile(load->Path("CharacterFairyHopeful_000_256x256.png"));
	success = gBitmapFont.buildFont(load->Path("font_000.png"));
	return success;
}

void close(Load* load)
{
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
	gModulatedTexture.Free();
	gWalkingSpriteSheetTexture.Free();
	gIconCursor.Free();
	gSun.Free();
	gMoon.Free();
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
	gPlayerBeam.Free();
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
		printf("Error: Unable to save file! %s\n", SDL_GetError());
	}
	gBitmapFont.Free();

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
		DestroySurface(loadedSurface);
	}

	return newTexture;
}

int main(int argc, char* argv[])
{
	Clock* clock = new Clock();
	Timer* myTimer = new Timer(Timer::Print, 1);
	const char* basePath = BasePath(argv[0]);
	Load* load = new Load(basePath);

	if (!Init())
	{
		SDL_Log("Failed to initialize!\n");
	}
	else
	{
		if (!loadMedia(load))
		{
			SDL_Log("Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			SDL_Event e;
			double degrees = 0;
#ifdef _WIN32
			SDL_FlipMode flipType = SDL_FLIP_NONE;
#elif __linux__
			SDL_RendererFlip flipType = SDL_FLIP_NONE;
#endif
			int xDir = 0;
			int yDir = 0;
			int countedFrames = 0;
			Dot dot;
			int backgroundScrollingOffset = 0;
			std::string inputText = "Input";
			int currentData = 0;
			double minuteAngle = 0;
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
			int gridCounter = 0;
			int playerI = 0;
			int playerJ = 0;
			int dotI = 0;
			int dotJ = 0;
			float dotNormalI = 0.0f;
			float dotNormalJ = 0.0f;
			SDL_Rect defaultRect = { -1, -1, -1, -1 };
			SDL_Rect beamViewport = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
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
				double secondNormal = (double)secondDelta / secondLimit;
				double trisecondNormal = (double)trisecondDelta / trisecondLimit;
				double minuteNormal = (double)minuteDelta / minuteLimit;
				previousTime = currentTime;

				SDL_GetMouseState(&mouseX, &mouseY);
				myTimer->Update();
				while (SDL_PollEvent(&e) != 0)
				{
//#ifdef __linux__
//					if (e.type == SDL_WINDOWEVENT) {
//						if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
//							gWindow.SetWidth(e.window.data1);
//							gWindow.SetHeight(e.window.data2);
//							printf("TODO, LINUX: THIS IS IN 2 DIFFERENT FILES width: %i, height: %i\n", gWindow.GetWidth(), gWindow.GetHeight());
//						}
//					}
//#endif
				  
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
						switch (Key(e))
						{
						case SDLK_HOME:
							break;
						}
					}
					else if (e.type == KEY_PRESSED)
					{
						switch (Key(e))
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
							case DIRECTION_TOTAL:
								break;
							}
#ifdef _WIN32
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
							break;
						case SDLK_LEFT:
						case KEY_A:
							gBox = &gBoxes[KEY_PRESS_SURFACE_LEFT];
							--gData[currentData];
							--playerJ;
							if (playerJ < 0) playerJ = 0;
							degrees -= 60;
							break;
						case SDLK_RIGHT:
						case KEY_D:
							gBox = &gBoxes[KEY_PRESS_SURFACE_RIGHT];
							++gData[currentData];
							++playerJ;
							if (playerJ >= ROW_SIZE) playerJ = ROW_SIZE - 1;
							degrees += 60;
							break;
						default:
							break;
						}
					}

#ifdef _WIN32
					if (e.type == SDL_EVENT_KEY_DOWN)
#elif __linux__
					if (e.type == KEY_PRESSED)
#endif
					{
						if (Key(e) == SDLK_BACKSPACE && inputText.length() > 0)
						{
							inputText.pop_back();
						}
					}
					dot.HandleEvent(e);
					gWindow.HandleEvent(gRenderer, e);
				}
				if (!gWindow.IsMinimized())
				{
					SDL_RenderClear(gRenderer);

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
					case DIRECTION_TOTAL:
						break;
					}

					minuteAngle = 360 * minuteNormal;

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
					case DIRECTION_TOTAL:
						break;
					}
					SDL_Rect fullscreenViewport =
					{
						fullscreenViewport.x = 0,
						fullscreenViewport.y = 0,
						fullscreenViewport.w = gWindow.GetWidth(),
						fullscreenViewport.h = gWindow.GetHeight()
					};
					SetRenderViewport(gRenderer, &fullscreenViewport);
					gModulatedTexture.render(0, 0);
					RenderTexture(gRenderer, gModulatedTexture.getTexture());
					dot.move((double)gWindow.GetWidth() * 0.000025, (double)gWindow.GetHeight() * 0.000025);
					dot.setIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ);

					SDL_Rect middleViewport =
					{
						middleViewport.x = (gWindow.GetWidth() * 0.5f) - 100,
						middleViewport.y = (gWindow.GetHeight() * 0.5f) - 100,
						middleViewport.w = 200,
						middleViewport.h = 200
					};
					SetRenderViewport(gRenderer, NULL);
					float normal = secondNormal;
					float centerX = mouseX;
					if (centerX > gWindow.GetWidth() * 0.15f) centerX = gWindow.GetWidth() * 0.15f;
					float centerY = gWindow.GetHeight() - mouseY;
					if (centerY > gWindow.GetHeight() * 0.5f) centerY = gWindow.GetHeight() * 0.5f;
					int horizonI = 0;
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
						SDL_Rect farLeft = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
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
							Distance distance = { (float)middleViewport.w, (float)middleViewport.h };
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
							if (j == 0)
							{
								farLeft.x = middleViewport.x;
								farLeft.y = middleViewport.y;
								farLeft.w = middleViewport.w;
								farLeft.h = middleViewport.h;
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
								SetRenderViewport(gRenderer, &middleViewport);
							}

							SDL_Rect walkingSpriteViewport =
							{
								walkingSpriteViewport.x = farLeft.x + ((farLeft.w * ROW_SIZE) * dotNormalJ) - (farLeft.w / 3),
								walkingSpriteViewport.y = farLeft.y - (farLeft.h * 0.5f),
								walkingSpriteViewport.w = farLeft.w / 2,
								walkingSpriteViewport.h = farLeft.h / 2
							};
							if (dotI == i)
							{
								beamViewport.x = walkingSpriteViewport.x;
								beamViewport.y = walkingSpriteViewport.y + walkingSpriteViewport.h;
								beamViewport.w = walkingSpriteViewport.w;
								beamViewport.h = walkingSpriteViewport.h;
								SetRenderViewport(gRenderer, &beamViewport);
								RenderTexture(gRenderer, gPlayerBeam.getTexture());
								SetRenderViewport(gRenderer, &walkingSpriteViewport);
								RenderTexture(gRenderer, gBox->getTexture());
								SetRenderViewport(gRenderer, &walkingSpriteViewport);
							}
						}
					}

					Point beamPoint =
					{
						(int)beamViewport.x + ((int)beamViewport.w / 2.0f),
						(int)beamViewport.y + ((int)beamViewport.h / 2.0f)
					};
					Point linePoint =
					{
						(dot.getBox().x + beamPoint.x) / 2,
						(dot.getBox().y + beamPoint.y) / 2
					};
					SDL_Rect lineViewport =
					{
						(int)linePoint.x,
						(int)linePoint.y,
						100,
						100
					};
					SetRenderViewport(gRenderer, &lineViewport);
					RenderTexture(gRenderer, characterFairyHopeful.getTexture());
					SetRenderViewport(gRenderer, &fullscreenViewport);
					RenderLine(
						gRenderer,
						dot.getBox().x + (dot.getBox().w * 2.5f),
						dot.getBox().y + (dot.getBox().h * 2.5f),
						beamPoint.x,
						beamPoint.y);
					dot.render();
					SetRenderViewport(gRenderer, NULL);

					if (isDebug)
					{
						gBitmapFont.renderText(0, 0, std::to_string(gData[currentData]).c_str());
						RenderLine(gRenderer, mouseX, mouseY, 0, 0);
						RenderLine(gRenderer, mouseX, mouseY, gWindow.GetWidth(), 0);
						RenderLine(gRenderer, mouseX, mouseY, 0, gWindow.GetHeight());
						RenderLine(gRenderer, mouseX, mouseY, gWindow.GetWidth(), gWindow.GetHeight());
					}
					SDL_RenderPresent(gRenderer);
				}
				++countedFrames;
			}
		}
		close(load);
	}

	delete load;
	delete[] basePath;
	delete myTimer;
	delete clock;
	return 0;
}
