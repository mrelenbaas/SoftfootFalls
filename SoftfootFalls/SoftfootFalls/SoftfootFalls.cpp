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
#include "Window.h"

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
const int TOTAL_DATA = 10;
const double PI = 3.14159265358979323846;

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

bool SecondInit();
bool loadMedia(Load*);
void close(Load*);
SDL_Texture* loadTexture(const char* path, Load* load, bool* success);

Window gWindow;
LTexture gModulatedTexture;
const int WALKING_ANIMATION_FRAMES = 4;
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
LTexture gPalaceTexture;
LTexture gBGTexture;
const int HORIZON_SIZE = 8;
LTexture gHorizons[HORIZON_SIZE];
const int ROAD_SIZE = 5;
LTexture gRoads[ROAD_SIZE];
LTexture gBoxes[KEY_PRESS_SURFACE_TOTAL];
LTexture* gBox = &gBoxes[KEY_PRESS_SURFACE_UP];
LTexture gBoxFront;
LTexture* gMiracleStarfalls;
int gMiracleStarfallIndex = 0;
int gMiracleStarfallModIndex = 0;
const int gMiracleStarfallLimit = 120;
bool gMiracleStarfallUpdateAtEndOfFrame = false;
LTexture gPlayerHighlight;
LTexture gPalaceHighlight;
LTexture gCharacterTownspersonMoonboy;
LTexture gCharacterTownspersonMoonboyHands;
LTexture gCharacterMonsterMouth;
LTexture gPlayerBeam;
LTexture characterFairyHopeful;
Sint32 gData[TOTAL_DATA];
LBitmapFont gBitmapFont;
Directions gDirection = DIRECTION_UP;
const int ROW_SIZE = 25;


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

LBitmapFont::LBitmapFont()
{
	mNewLine = 0;
	mSpace = 0;
}

bool LBitmapFont::buildFont(std::string path)
{
	Free();
	bool success = true;
	mFontTexture.Init(gWindow.GetRenderer());
	if (!mFontTexture.LoadPixelsFromFile(path.c_str()))
	{
		SDL_Log("Unable to load bitmap font surface!\n");
		success = false;
	}
	else
	{
		Uint32 bgColor = mFontTexture.GetPixel32(0, 0);
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
						if (mFontTexture.GetPixel32(pX, pY) != bgColor)
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
						if (mFontTexture.GetPixel32(pX, pY) != bgColor)
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
						if (mFontTexture.GetPixel32(pX, pY) != bgColor)
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
							if (mFontTexture.GetPixel32(pX, pY) != bgColor)
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
		if (!mFontTexture.LoadFromPixels())
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
				mFontTexture.Render(curX, curY, &mChars[ascii]);
				curX += mChars[ascii].w + 1;
			}
		}
	}
}

bool SecondInit()
{
	bool success = true;

	if (FirstInit())
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
		if (!gWindow.Init())
		{
			SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
#ifdef __linux__
			gWindow.SetRenderer(SDL_CreateRenderer(gWindow.GetWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
			if (gWindow.GetRenderer() == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(gWindow.GetRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
#endif
		}
	}

	return success;
}

bool loadMedia(Load* load)
{
	using namespace std;

	bool success = true;

	gModulatedTexture.Init(gWindow.GetRenderer());
	if (!(success = gModulatedTexture.LoadFromFile(load->Path("Landscape_Moon_3300x2550.png")))) {}
	gIconCursor.Init(gWindow.GetRenderer());
	success = gIconCursor.LoadFromFile(load->Path("IconCursor.png"));
	gSun.Init(gWindow.GetRenderer());
	success = gSun.LoadFromFile(load->Path("CharacterFairySun_000_256x256.png"));
	gMoon.Init(gWindow.GetRenderer());
	success = gMoon.LoadFromFile(load->Path("CharacterFairyMoon_000_256x256.png"));
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

	gDotTexture.Init(gWindow.GetRenderer());
	success = gDotTexture.LoadFromFile(load->Path("CharacterFairySmallwig000_64x64.png"));
	gPalaceTexture.Init(gWindow.GetRenderer());
	success = gPalaceTexture.LoadFromFile(load->Path("BoxFront.png"));
	gBGTexture.Init(gWindow.GetRenderer());
	success = gBGTexture.LoadFromFile(load->Path("bg.png"));
	gHorizons[0].Init(gWindow.GetRenderer());
	success = gHorizons[0].LoadFromFile(load->Path("Horizon000.png"));
	gHorizons[1].Init(gWindow.GetRenderer());
	success = gHorizons[1].LoadFromFile(load->Path("Horizon001.png"));
	gHorizons[2].Init(gWindow.GetRenderer());
	success = gHorizons[2].LoadFromFile(load->Path("Horizon002.png"));
	gHorizons[3].Init(gWindow.GetRenderer());
	success = gHorizons[3].LoadFromFile(load->Path("Horizon003.png"));
	gHorizons[4].Init(gWindow.GetRenderer());
	success = gHorizons[4].LoadFromFile(load->Path("Horizon004.png"));
	gHorizons[5].Init(gWindow.GetRenderer());
	success = gHorizons[5].LoadFromFile(load->Path("Horizon005.png"));
	gHorizons[6].Init(gWindow.GetRenderer());
	success = gHorizons[6].LoadFromFile(load->Path("Horizon006.png"));
	gHorizons[7].Init(gWindow.GetRenderer());
	success = gHorizons[7].LoadFromFile(load->Path("Horizon007.png"));
	gRoads[0].Init(gWindow.GetRenderer());
	success = gRoads[0].LoadFromFile(load->Path("Road_000_256x256.png"));
	gRoads[1].Init(gWindow.GetRenderer());
	success = gRoads[1].LoadFromFile(load->Path("Road_001_256x256.png"));
	gRoads[2].Init(gWindow.GetRenderer());
	success = gRoads[2].LoadFromFile(load->Path("Road_002_256x256.png"));
	gRoads[3].Init(gWindow.GetRenderer());
	success = gRoads[3].LoadFromFile(load->Path("Road_003_256x256.png"));
	gRoads[4].Init(gWindow.GetRenderer());
	success = gRoads[4].LoadFromFile(load->Path("Road_004_256x256.png"));
	gBoxes[KEY_PRESS_SURFACE_UP].Init(gWindow.GetRenderer());
	success = gBoxes[KEY_PRESS_SURFACE_UP].LoadFromFile(load->Path("BoxUp.png"));
	gBoxes[KEY_PRESS_SURFACE_DOWN].Init(gWindow.GetRenderer());
	success = gBoxes[KEY_PRESS_SURFACE_DOWN].LoadFromFile(load->Path("BoxDown.png"));
	gBoxes[KEY_PRESS_SURFACE_LEFT].Init(gWindow.GetRenderer());
	success = gBoxes[KEY_PRESS_SURFACE_LEFT].LoadFromFile(load->Path("BoxLeft.png"));
	gBoxes[KEY_PRESS_SURFACE_RIGHT].Init(gWindow.GetRenderer());
	success = gBoxes[KEY_PRESS_SURFACE_RIGHT].LoadFromFile(load->Path("BoxRight.png"));
	gBoxFront.Init(gWindow.GetRenderer());
	success = gBoxFront.LoadFromFile(load->Path("BoxFront.png"));
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		int j = i % 5;
		if (j == 0)
		{
			gMiracleStarfalls[i].Init(gWindow.GetRenderer());
			success = gMiracleStarfalls[i].LoadFromFile(load->Path("MiracleStarfall_000_1024x1024.png"));
		}
		else if (j == 1)
		{
			gMiracleStarfalls[i].Init(gWindow.GetRenderer());
			success = gMiracleStarfalls[i].LoadFromFile(load->Path("MiracleStarfall_001_1024x1024.png"));
		}
		else if (j == 2)
		{
			gMiracleStarfalls[i].Init(gWindow.GetRenderer());
			success = gMiracleStarfalls[i].LoadFromFile(load->Path("MiracleStarfall_002_1024x1024.png"));
		}
		else if (j == 3)
		{
			gMiracleStarfalls[i].Init(gWindow.GetRenderer());
			success = gMiracleStarfalls[i].LoadFromFile(load->Path("MiracleStarfall_003_1024x1024.png"));
		}
		else if (j == 4)
		{
			gMiracleStarfalls[i].Init(gWindow.GetRenderer());
			success = gMiracleStarfalls[i].LoadFromFile(load->Path("MiracleStarfall_004_1024x1024.png"));
		}
	}
	gPlayerHighlight.Init(gWindow.GetRenderer());
	success = gPlayerHighlight.LoadFromFile(load->Path("PlayerHighlight_000_1024x1024.png"));
	gPalaceHighlight.Init(gWindow.GetRenderer());
	success = gPalaceHighlight.LoadFromFile(load->Path("PalaceHighlight_000_2048x2048.png"));
	gCharacterTownspersonMoonboy.Init(gWindow.GetRenderer());
	success = gCharacterTownspersonMoonboy.LoadFromFile(load->Path("CharacterTownspersonMoonboy_000_1024x1024.png"));
	gCharacterTownspersonMoonboyHands.Init(gWindow.GetRenderer());
	success = gCharacterTownspersonMoonboyHands.LoadFromFile(load->Path("CharacterTownspersonMoonboy_Hands_000_1024x1024.png"));
	gCharacterMonsterMouth.Init(gWindow.GetRenderer());
	success = gCharacterMonsterMouth.LoadFromFile(load->Path("CharacterMonsterMouth_000_1024x1024.png"));
	gPlayerBeam.Init(gWindow.GetRenderer());
	success = gPlayerBeam.LoadFromFile(load->Path("PlayerBeam_000_2048x2048.png"));
	characterFairyHopeful.Init(gWindow.GetRenderer());
	success = characterFairyHopeful.LoadFromFile(load->Path("CharacterFairyHopeful_000_256x256.png"));
	success = gBitmapFont.buildFont(load->Path("font_000.png"));
	return success;
}

void close(Load* load)
{
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
	gPalaceTexture.Free();
	gBGTexture.Free();
	for (int i = 0; i < HORIZON_SIZE; ++i)
	{
		gHorizons[i].Free();
	}
	for (int i = 0; i < ROAD_SIZE; ++i)
	{
		gRoads[i].Free();
	}
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
	{
		gBoxes[i].Free();
	}
	gBoxFront.Free();
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfalls[i].Free();
	}
	gPlayerHighlight.Free();
	gPalaceHighlight.Free();
	gCharacterTownspersonMoonboy.Free();
	gCharacterTownspersonMoonboyHands.Free();
	gCharacterMonsterMouth.Free();
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

	SDL_DestroyRenderer(gWindow.GetRenderer());
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
		newTexture = SDL_CreateTextureFromSurface(gWindow.GetRenderer(), loadedSurface);
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

	gMiracleStarfalls = new LTexture[gMiracleStarfallLimit];
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfalls[i].Init(gWindow.GetRenderer());
	}
	SDL_Rect gMiracleStarfallViewports[gMiracleStarfallLimit];
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallViewports[i].x = 0;
		gMiracleStarfallViewports[i].y = 0;
		gMiracleStarfallViewports[i].w = 0;
		gMiracleStarfallViewports[i].h = 0;
	}
	bool gMiracleStarfallSpawns[gMiracleStarfallLimit];
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallSpawns[i] = false;
	}
	bool gMiracleStarfallDespawns[gMiracleStarfallLimit];
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallDespawns[i] = false;
	}
	long long gMiracleStarfallDeltas[gMiracleStarfallLimit];
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallDeltas[i] = 0L;
	}
	double gMiracleStarfallNormals[gMiracleStarfallLimit];
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallNormals[i] = 0;
	}

	if (!SecondInit())
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
			long long twelvesecondDelta = 0L;
			long long minuteDelta = 0L;
			long long hourDelta = 0L;
			long long halfDayDelta = 0L;
			long long fullDayDelta = 0L;
			long long playerDelta = 0L;
			bool trisecondToggle = false;
			bool twelvesecondToggle = false;
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
				twelvesecondDelta += currentTime - previousTime;
				minuteDelta += currentTime - previousTime;
				hourDelta += currentTime - previousTime;
				halfDayDelta += currentTime - previousTime;
				fullDayDelta += currentTime - previousTime;
				for (int m = 0; m < gMiracleStarfallLimit; ++m)
				{
					gMiracleStarfallDeltas[m] += currentTime - previousTime;
				}

				double decisecondLimit = 100'000'000;
				double secondLimit = 1'000'000'000;
				double trisecondLimit = 3'000'000'000;
				double twelvesecondLimit = 6'000'000'000;
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
				if (twelvesecondDelta > twelvesecondLimit)
				{
					twelvesecondToggle = !twelvesecondToggle;
					twelvesecondDelta -= twelvesecondLimit;
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
				double twelvesecondNormal = (double)twelvesecondDelta / twelvesecondLimit;
				double minuteNormal = (double)minuteDelta / minuteLimit;
				for (int m = 0; m < gMiracleStarfallLimit; ++m)
				{
					gMiracleStarfallNormals[m] = (double)gMiracleStarfallDeltas[m] / secondLimit;
					if (gMiracleStarfallNormals[m] > 1.0)
					{
						gMiracleStarfallDeltas[m] = 0L;
						gMiracleStarfallDespawns[m] = false;
					}
				}
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
						case KEY_Q:
							gMiracleStarfallUpdateAtEndOfFrame = true;
							gMiracleStarfallModIndex = gMiracleStarfallIndex % 5;
							gMiracleStarfallSpawns[gMiracleStarfallIndex] = true;
							gMiracleStarfallDeltas[gMiracleStarfallIndex] = 0L;
							gMiracleStarfallNormals[gMiracleStarfallIndex] = (double)gMiracleStarfallDeltas[gMiracleStarfallIndex] / secondLimit;
							break;
						case KEY_E:
							break;
						default:
							break;
						}
					}
					if (e.type == KEY_PRESSED)
					{
						if (Key(e) == SDLK_BACKSPACE && inputText.length() > 0)
						{
							inputText.pop_back();
						}
					}
					dot.HandleEvent(e);
					gWindow.HandleEvent(e);
				}
				if (!gWindow.IsMinimized())
				{
					SDL_RenderClear(gWindow.GetRenderer());

					double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / PI);
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
						gBGTexture.Render(0, backgroundScrollingOffset);
						gBGTexture.Render(0, backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.Render(0, backgroundScrollingOffset + (gBGTexture.GetHeight() * 2));
						gBGTexture.Render(gBGTexture.GetWidth(), backgroundScrollingOffset);
						gBGTexture.Render(gBGTexture.GetWidth(), backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.Render(gBGTexture.GetWidth(), backgroundScrollingOffset + (gBGTexture.GetHeight() * 2));
						break;
					case DIRECTION_DOWN:
						gBGTexture.Render(0, backgroundScrollingOffset);
						gBGTexture.Render(0, backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.Render(0, backgroundScrollingOffset - gBGTexture.GetHeight());
						gBGTexture.Render(gBGTexture.GetWidth(), backgroundScrollingOffset);
						gBGTexture.Render(gBGTexture.GetWidth(), backgroundScrollingOffset + gBGTexture.GetHeight());
						gBGTexture.Render(gBGTexture.GetWidth(), backgroundScrollingOffset - gBGTexture.GetHeight());
						break;
					case DIRECTION_LEFT:
						gBGTexture.Render(backgroundScrollingOffset, 0);
						gBGTexture.Render(backgroundScrollingOffset + gBGTexture.GetWidth(), 0);
						gBGTexture.Render(backgroundScrollingOffset + (gBGTexture.GetWidth() * 2), 0);
						gBGTexture.Render(backgroundScrollingOffset, gBGTexture.GetHeight());
						gBGTexture.Render(backgroundScrollingOffset + gBGTexture.GetWidth(), gBGTexture.GetHeight());
						gBGTexture.Render(backgroundScrollingOffset + (gBGTexture.GetWidth() * 2), gBGTexture.GetHeight());
						break;
					case DIRECTION_RIGHT:
						gBGTexture.Render(backgroundScrollingOffset, 0);
						gBGTexture.Render(backgroundScrollingOffset + gBGTexture.GetWidth(), 0);
						gBGTexture.Render(backgroundScrollingOffset - gBGTexture.GetWidth(), 0);
						gBGTexture.Render(backgroundScrollingOffset, gBGTexture.GetHeight());
						gBGTexture.Render(backgroundScrollingOffset + gBGTexture.GetWidth(), gBGTexture.GetHeight());
						gBGTexture.Render(backgroundScrollingOffset - gBGTexture.GetWidth(), gBGTexture.GetHeight());
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
					int horizontalModifier = 0;
					int verticalModifier = 0;
					if (twelvesecondToggle)
					{
						horizontalModifier = (gWindow.GetWidth() + fullscreenViewport.w) * twelvesecondNormal;
						verticalModifier = (gWindow.GetHeight() + fullscreenViewport.h) * twelvesecondNormal;
					}
					else
					{
						horizontalModifier = (gWindow.GetWidth() + fullscreenViewport.w) * (1.0f - twelvesecondNormal);
						verticalModifier = (gWindow.GetHeight() + fullscreenViewport.h) * (1.0f - twelvesecondNormal);
					}
					SDL_Rect characterViewport =
					{
						characterViewport.x = fullscreenViewport.x - fullscreenViewport.w + horizontalModifier,
						characterViewport.y = fullscreenViewport.y - (fullscreenViewport.h / 2),
						characterViewport.w = fullscreenViewport.w,
						characterViewport.h = fullscreenViewport.h
					};
					SetRenderViewport(gWindow.GetRenderer(), &characterViewport);
					RenderTexture(gWindow.GetRenderer(), gCharacterTownspersonMoonboy.GetTexture());
					SetRenderViewport(gWindow.GetRenderer(), &fullscreenViewport);
					RenderTexture(gWindow.GetRenderer(), gModulatedTexture.GetTexture());
					SetRenderViewport(gWindow.GetRenderer(), &characterViewport);
					RenderTexture(gWindow.GetRenderer(), gCharacterTownspersonMoonboyHands.GetTexture());
					dot.move((double)gWindow.GetWidth() * 0.00005, (double)gWindow.GetHeight() * 0.00005);
					dot.setIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ);

					SDL_Rect middleViewport =
					{
						middleViewport.x = (gWindow.GetWidth() * 0.5f) - 100,
						middleViewport.y = (gWindow.GetHeight() * 0.5f) - 100,
						middleViewport.w = 200,
						middleViewport.h = 200
					};
					SetRenderViewport(gWindow.GetRenderer(), NULL);
					float normal = secondNormal;
					float centerX = mouseX;
					if (centerX > gWindow.GetWidth() * 0.15f) centerX = gWindow.GetWidth() * 0.15f;
					float centerY = gWindow.GetHeight() - mouseY;
					if (centerY > gWindow.GetHeight() * 0.5f) centerY = gWindow.GetHeight() * 0.5f;
					int horizonI = 0;
					int roadI = 0;
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
							SetRenderViewport(gWindow.GetRenderer(), &middleViewport);
							int modI = (i % 2 == 0) ? 0 : 4;
							RenderTexture(gWindow.GetRenderer(), gHorizons[modI + horizonI].GetTexture());
							++horizonI;
							if (horizonI > 3)
							{
								horizonI = 0;
							}
							modI = (i % 4 == 0);
							RenderTexture(gWindow.GetRenderer(), gRoads[modI + roadI].GetTexture());
							++roadI;
							if (roadI > 3)
							{
								roadI = 0;
							}
							if (dotI == i && dotJ == j)
							{
								RenderTexture(gWindow.GetRenderer(), gPlayerHighlight.GetTexture());
							}
							SetRenderViewport(gWindow.GetRenderer(), &middleViewport);
							Distance distance = { (float)middleViewport.w, (float)middleViewport.h };
							if (i == 0 && j == 0)
							{
								gIconCursor.Render(
									0,
									0,
									NULL,
									joystickAngle,
									NULL,
									SDL_FLIP_NONE,
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
								gIconCursor.Render(
									0,
									0,
									NULL,
									degrees,
									NULL,
									SDL_FLIP_NONE,
									distance);
							}
							if (i == ROW_SIZE - 1 && j == 0)
							{
								gSun.Render(
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
								gMoon.Render(
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
								SetRenderViewport(gWindow.GetRenderer(), NULL);
								int x = 0;
								int y = middleViewport.y - (middleViewport.h * 2.0f);
								if (y < 0)
								{
									y = 0;
								}
								SetRenderViewport(gWindow.GetRenderer(), &middleViewport);
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
								SetRenderViewport(gWindow.GetRenderer(), &beamViewport);
								RenderTexture(gWindow.GetRenderer(), gPlayerBeam.GetTexture());
								SetRenderViewport(gWindow.GetRenderer(), &walkingSpriteViewport);
								RenderTexture(gWindow.GetRenderer(), gBox->GetTexture());
								SetRenderViewport(gWindow.GetRenderer(), &walkingSpriteViewport);
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
					SetRenderViewport(gWindow.GetRenderer(), &lineViewport);
					RenderTexture(gWindow.GetRenderer(), characterFairyHopeful.GetTexture());
					SDL_Rect monsterViewport =
					{
						monsterViewport.x = fullscreenViewport.x,
						monsterViewport.y = fullscreenViewport.y - fullscreenViewport.h + verticalModifier,
						monsterViewport.w = fullscreenViewport.w,
						monsterViewport.h = fullscreenViewport.h
					};
					SetRenderViewport(gWindow.GetRenderer(), &monsterViewport);
					SetRenderViewport(gWindow.GetRenderer(), &fullscreenViewport);
					RenderLine(
						gWindow.GetRenderer(),
						dot.getBox().x + (dot.getBox().w * 2.5f),
						dot.getBox().y + (dot.getBox().h * 2.5f),
						beamPoint.x,
						beamPoint.y);
					SDL_Rect palaceViewport
					{
						dot.getBox().x - (dot.getBox().w / 4 * 10),
						dot.getBox().y - (dot.getBox().h / 4 * 10),
						dot.getBox().w * 10,
						dot.getBox().h * 10
					};
					SetRenderViewport(gWindow.GetRenderer(), &palaceViewport);
					RenderTexture(gWindow.GetRenderer(), gPalaceTexture.GetTexture());
					if (gMiracleStarfallSpawns[gMiracleStarfallIndex])
					{
						gMiracleStarfallViewports[gMiracleStarfallIndex].x = palaceViewport.x;
						gMiracleStarfallViewports[gMiracleStarfallIndex].y = palaceViewport.y;
						gMiracleStarfallViewports[gMiracleStarfallIndex].w = palaceViewport.w;
						gMiracleStarfallViewports[gMiracleStarfallIndex].h = palaceViewport.h;
						gMiracleStarfallDespawns[gMiracleStarfallIndex] = true;
						gMiracleStarfallSpawns[gMiracleStarfallIndex] = false;
					}
					for (int m = 0; m < gMiracleStarfallLimit; ++m)
					{
						if (gMiracleStarfallDeltas[m] > secondLimit)
						{
							gMiracleStarfallDespawns[m] = false;
						}
						if (gMiracleStarfallDespawns[m])
						{
							SDL_Rect currentStar = {
								gMiracleStarfallViewports[m].x,
								gMiracleStarfallViewports[m].y - (gWindow.GetHeight() * gMiracleStarfallNormals[m]),
								gMiracleStarfallViewports[m].w,
								gMiracleStarfallViewports[m].h
							};
							SetRenderViewport(gWindow.GetRenderer(), &currentStar);
							RenderTexture(gWindow.GetRenderer(), gMiracleStarfalls[m].GetTexture());
						}
					}
					SDL_Rect palaceHighlightViewport =
					{
						palaceViewport.x + palaceViewport.w,
						palaceViewport.y,
						palaceViewport.w,
						palaceViewport.h
					};
					SetRenderViewport(gWindow.GetRenderer(), &palaceHighlightViewport);
					RenderTexture(gWindow.GetRenderer(), gPalaceHighlight.GetTexture());
					SetRenderViewport(gWindow.GetRenderer(), NULL);

					if (isDebug)
					{
						gBitmapFont.renderText(0, 0, std::to_string(gData[currentData]).c_str());
						RenderLine(gWindow.GetRenderer(), mouseX, mouseY, 0, 0);
						RenderLine(gWindow.GetRenderer(), mouseX, mouseY, gWindow.GetWidth(), 0);
						RenderLine(gWindow.GetRenderer(), mouseX, mouseY, 0, gWindow.GetHeight());
						RenderLine(gWindow.GetRenderer(), mouseX, mouseY, gWindow.GetWidth(), gWindow.GetHeight());
					}
					SDL_RenderPresent(gWindow.GetRenderer());
				}
				if (gMiracleStarfallUpdateAtEndOfFrame)
				{
					++gMiracleStarfallIndex;
					if (gMiracleStarfallIndex >= gMiracleStarfallLimit)
					{
						gMiracleStarfallIndex = 0;
					}
					gMiracleStarfallUpdateAtEndOfFrame = false;
				}
				++countedFrames;
			}
		}
		close(load);
	}

	delete[] gMiracleStarfalls;

	delete load;
	delete[] basePath;
	delete myTimer;
	delete clock;
	return 0;
}
