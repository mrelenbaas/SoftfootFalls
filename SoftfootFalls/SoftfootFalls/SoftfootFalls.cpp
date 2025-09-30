#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <functional>
#include <string>
#include <cmath>

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
#include <SDL2/SDL_ttf.h>
#endif
#include "Load.h"

#ifdef _WIN32
#elif __linux__
#endif


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

const int JOYSTICK_DEAD_ZONE = 8000;
#ifdef _WIN32
const double M_PI = 3.14159265359;
#endif

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
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

class LTexture
{
public:
	LTexture();
	~LTexture();
	bool loadFromFile(const char* path);
#if defined(SDL_TTF_MAJOR_VERSION)
	bool loadFromRenderedText(const char* textureText, SDL_Color textColor);
#endif
	void free();
	void setColor(Uint8 red, Uint8 green, Uint8 blue);
	void setBlendMode(SDL_BlendMode blending);
	void setAlpha(Uint8 alpha);
#ifdef _WIN32
	void render(int x, int y, SDL_FRect* clip = NULL, double angle = 0.0, SDL_FPoint* center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE);
#elif __linux__
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
#endif
	int getWidth();
	int getHeight();
private:
	SDL_Texture* mTexture;
	int mWidth;
	int mHeight;
};

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

bool init();
bool loadMedia();
void close();
SDL_Surface* loadSurface(const char* path, Load* load, bool* success);
SDL_Texture* loadTexture(const char* path, Load* load, bool* success);

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface* gCurrentSurface = NULL;
SDL_Texture* gTexture = NULL;
LTexture gFooTexture;
#ifdef _WIN32
SDL_FRect gDotSpriteClips[4];
#elif __linux__
SDL_Rect gDotSpriteClips[4];
#endif
LTexture gDotSpriteSheetTexture;
LTexture gBackgroundTexture;
LTexture gModulatedTexture;
const int WALKING_ANIMATION_FRAMES = 4;
#ifdef _WIN32
SDL_FRect gWalkingSpriteClips[WALKING_ANIMATION_FRAMES];
#elif __linux__
SDL_Rect gWalkingSpriteClips[WALKING_ANIMATION_FRAMES];
#endif
LTexture gWalkingSpriteSheetTexture;
LTexture gArrowTexture;
TTF_Font* gFont = NULL;
LTexture gTextTexture;
#ifdef _WIN32
SDL_FRect gSpriteClips[BUTTON_SPRITE_TOTAL];
#elif __linux__
SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
#endif
LTexture gButtonSpriteSheetTexture;
LButton gButtons[TOTAL_BUTTONS];
LTexture gPressTexture;
LTexture gUpTexture;
LTexture gDownTexture;
LTexture gLeftTexture;
LTexture gRightTexture;
LTexture gSplashTexture;
#ifdef _WIN32
SDL_Gamepad* gGameController;
#elif __linux__
SDL_GameController* gGameController;
#endif
SDL_Joystick* gJoystick = NULL;
SDL_Haptic* gJoyHaptic = NULL;
//LTexture gPromptTexture;
//MIX_Mixer* gMusic = NULL;
//MIX_Audio* gBeat = NULL;
//MIX_Audio* gScratch = NULL;
//MIX_Audio* gHigh = NULL;
//MIX_Audio* gMedium = NULL;
//MIX_Audio* gLow = NULL;
static SDL_AudioStream* stream = NULL;
static int current_sine_sample = 0;



LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(const char* path)
{
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		SDL_Log("Unable to load image %s! SDL_image Error: %s\n", path, SDL_GetError());
	}
	else
	{
#ifdef _WIN32
		SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0, 0xFF, 0xFF));
#elif __linux__
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
#endif
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			SDL_Log("Unable to create texture from %s! SDL Error %s\n", path, SDL_GetError());
		}
		else
		{
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
#ifdef _WIN32
		SDL_DestroySurface(loadedSurface);
#elif __linux__
		SDL_FreeSurface(loadedSurface);
#endif
	}
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(const char* textureText, SDL_Color textColor)
{
	free();
#ifdef _WIN32
	SDL_Surface* textSurface = TTF_RenderText_Blended(gFont, textureText, 0, textColor);
#elif __linux__
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText, textColor);
#endif
	if (textSurface == NULL)
	{
		SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", SDL_GetError());
	}
	else
	{
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			SDL_Log("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
#ifdef _WIN32
		SDL_DestroySurface(textSurface);
#elif __linux__
		SDL_FreeSurface(textSurface);
#endif
	}
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
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
void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
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
	//SDL_RenderTexture(gRenderer, mTexture, clip, &renderQuad);
	SDL_RenderTextureRotated(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
#elif __linux__
	//SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
#endif
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
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
#ifdef _WIN32
		if (!SDL_CreateWindowAndRenderer("SDL Tutorial", SCREEN_WIDTH, SCREEN_HEIGHT, 0, &gWindow, &gRenderer))
#elif __linux__
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
#endif
		{
			SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
#ifdef _WIN32
			//SDL_AudioSpec audio_spec;
			SDL_SetRenderVSync(gRenderer, 1);
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
#elif __linux__
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}

bool loadMedia()
{
	using namespace std;

	Load* load = new Load(SDL_GetBasePath());
	bool success = true;

	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface(load->Path("press.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface(load->Path("up.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface(load->Path("down.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface(load->Path("left.png"), load, &success);
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface(load->Path("right.png"), load, &success);
	gTexture = loadTexture(load->Path("texture.png"), load, &success);
	if (!gFooTexture.loadFromFile(load->Path("foo2.png")))
	{
		SDL_Log("Failed to load foo' texture image!\n");
		success = false;
	}
	if (!gBackgroundTexture.loadFromFile(load->Path("background.png")))
	{
		SDL_Log("Failed to load background texture image!\n");
		success = false;
	}
	if (!gDotSpriteSheetTexture.loadFromFile(load->Path("dots.png")))
	{
		SDL_Log("Failed to load sprite sheet texture!\n");
		success = false;
	}
	else
	{
		gDotSpriteClips[0].x = 0;
		gDotSpriteClips[0].y = 0;
		gDotSpriteClips[0].w = 100;
		gDotSpriteClips[0].h = 100;
		gDotSpriteClips[1].x = 100;
		gDotSpriteClips[1].y = 0;
		gDotSpriteClips[1].w = 100;
		gDotSpriteClips[1].h = 100;
		gDotSpriteClips[2].x = 0;
		gDotSpriteClips[2].y = 100;
		gDotSpriteClips[2].w = 100;
		gDotSpriteClips[2].h = 100;
		gDotSpriteClips[3].x = 100;
		gDotSpriteClips[3].y = 100;
		gDotSpriteClips[3].w = 100;
		gDotSpriteClips[3].h = 100;
	}
	if (!gModulatedTexture.loadFromFile(load->Path("fadeout.png")))
	{
		SDL_Log("Failed to load front texture!\n");
		success = false;
	}
	else
	{
		gModulatedTexture.setBlendMode(SDL_BLENDMODE_BLEND);
	}
	if (!gBackgroundTexture.loadFromFile(load->Path("fadein.png")))
	{
		SDL_Log("Failed to load background texture!\n");
		success = false;
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
	if (!gArrowTexture.loadFromFile(load->Path("arrow.png")))
	{
		SDL_Log("Failed to load arrow texture!\n");
		success = false;
	}
	gFont = TTF_OpenFont(load->Path("lazy.ttf"), 28);
	if (gFont == NULL)
	{
		SDL_Log("Failed to load lazy font! SDL_ttf Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		SDL_Color textColor = { 0, 0, 0, 0 };
		if (!gTextTexture.loadFromRenderedText("The quick brown fox jumps over the lazy dog", textColor))
		{
			SDL_Log("Failed to render text texture!\n");
			success = false;
		}
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
	if (!gPressTexture.loadFromFile(load->Path("press.png")))
	{
		SDL_Log("Failed to load press texture!\n");
		success = false;
	}
	if (!gUpTexture.loadFromFile(load->Path("up.png")))
	{
		SDL_Log("Failed to load up texture!\n");
		success = false;
	}
	if (!gDownTexture.loadFromFile(load->Path("down.png")))
	{
		SDL_Log("Failed to load down texture!\n");
		success = false;
	}
	if (!gLeftTexture.loadFromFile(load->Path("left.png")))
	{
		SDL_Log("Failed to load left texture!\n");
		success = false;
	}
	if (!gRightTexture.loadFromFile(load->Path("right.png")))
	{
		SDL_Log("Failed to load right texture!\n");
		success = false;
	}
	if (!gSplashTexture.loadFromFile(load->Path("splash.png")))
	{
		SDL_Log("Failed to load splash texture!\n");
		success = false;
	}
	/*if (!gPromptTexture.loadFromFile(load->Path("prompt.png")))
	{
		SDL_Log("Failed to load prompt texture!\n");
		success = false;
	}
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

	delete load;
	return success;
}

void close()
{
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i)
	{
#ifdef _WIN32
		SDL_DestroySurface(gKeyPressSurfaces[i]);
#elif __linux__
		SDL_FreeSurface(gKeyPressSurfaces[i]);
#endif
		gKeyPressSurfaces[i] = NULL;
	}
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;
	gFooTexture.free();
	gBackgroundTexture.free();
	gDotSpriteSheetTexture.free();
	gModulatedTexture.free();
	gBackgroundTexture.free();
	gWalkingSpriteSheetTexture.free();
	gArrowTexture.free();
	gTextTexture.free();
	TTF_CloseFont(gFont);
	gFont = NULL;
	gButtonSpriteSheetTexture.free();
	gPressTexture.free();
	gUpTexture.free();
	gDownTexture.free();
	gLeftTexture.free();
	gRightTexture.free();
	gSplashTexture.free();
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
	//gPromptTexture.free();
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

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	TTF_Quit();
	MIX_Quit();
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
	Timer* timer = new Timer(Timer::Print, 1);

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
			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
			Uint8 r = 255;
			Uint8 g = 255;
			Uint8 b = 255;
			Uint8 a = 255;
			int frame = 0;
			double degrees = 0;
#ifdef _WIN32
			SDL_FlipMode flipType = SDL_FLIP_NONE;
#elif __linux__
			SDL_RendererFlip flipType = SDL_FLIP_NONE;
#endif
			LTexture* currentTexture;
			int xDir = 0;
			int yDir = 0;
			while (!quit)
			{
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
				timer->Update();
				while (SDL_PollEvent(&e) != 0)
				{
#ifdef _WIN32
					if (e.type == SDL_EVENT_QUIT)
#elif __linux__
					if (e.type == SDL_QUIT)
#endif
						quit = true;
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
					else if (e.key.key)
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
#ifdef _WIN32
						case SDLK_Q:
#elif __linux__
						case SDLK_q:
#endif
							r += 32;
							flipType = SDL_FLIP_HORIZONTAL;
							break;
#ifdef _WIN32
						case SDLK_W:
#elif __linux__
						case SDLK_w:
#endif
							g += 32;
							flipType = SDL_FLIP_NONE;
							break;
#ifdef _WIN32
						case SDLK_E:
#elif __linux__
						case SDLK_e:
#endif
							b += 32;
							flipType = SDL_FLIP_VERTICAL;
							break;
#ifdef _WIN32
						case SDLK_A:
#elif __linux__
						case SDLK_a:
#endif
							r -= 32;
							degrees -= 60;
							break;
#ifdef _WIN32
						case SDLK_S:
#elif __linux__
						case SDLK_s:
#endif
							g -= 32;
							break;
#ifdef _WIN32
						case SDLK_D:
#elif __linux__
						case SDLK_d:
#endif
							b -= 32;
							degrees += 60;
							break;
#ifdef _WIN32
						case SDLK_R:
#elif __linux__
						case SDLK_r:
#endif
							if (a + 32 > 255)
							{
								a = 255;
							}
							else
							{
								a += 32;
							}
							break;
#ifdef _WIN32
						case SDLK_F:
#elif __linux__
						case SDLK_f:
#endif
							if (a - 32 < 0)
							{
								a = 0;
							}
							else
							{
								a -= 32;
							}
							break;
						case SDLK_UP:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
							break;
						case SDLK_DOWN:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
							break;
						case SDLK_LEFT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
							break;
						case SDLK_RIGHT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
							break;
						default:
							//gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
							gCurrentSurface = NULL;
							break;
						}
					}
					for (int i = 0; i < TOTAL_BUTTONS; ++i)
					{
						gButtons[i].handleEvent(&e);
					}
				}
				SDL_RenderClear(gRenderer);
				SDL_Rect stretchRect;
				stretchRect.x = 0;
				stretchRect.y = 0;
				stretchRect.w = SCREEN_WIDTH;
				stretchRect.h = SCREEN_HEIGHT;
#ifdef _WIN32
				SDL_BlitSurfaceScaled(gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT], NULL, gScreenSurface, &stretchRect, SDL_SCALEMODE_NEAREST);
#elif __linux__
				SDL_BlitSurface(gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT], NULL, gScreenSurface, NULL);
#endif
#ifdef _WIN32
				SDL_BlitSurfaceScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect, SDL_SCALEMODE_NEAREST);
#elif __linux__
				SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);
#endif

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

#ifdef _WIN32
				SDL_RenderTexture(gRenderer, gTexture, NULL, NULL);
#elif __linux__
				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
#endif

#ifdef _WIN32
#elif __linux__
#endif

#ifdef _WIN32
				SDL_FRect fillRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
#elif __linux__
				SDL_Rect fillRect = {SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
#endif
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
				SDL_RenderFillRect(gRenderer, &fillRect);

#ifdef _WIN32
				SDL_FRect outlineRect = {SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3};
#elif __linux__
				SDL_Rect outlineRect = {SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3};
#endif
				SDL_SetRenderDrawColor(gRenderer, 0x00, 0xFF, 0x00, 0xFF);
#ifdef _WIN32
				SDL_RenderRect(gRenderer, &outlineRect);
#elif __linux__
				SDL_RenderDrawRect(gRenderer, &outlineRect);
#endif

				SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
#ifdef _WIN32
				SDL_RenderLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
#elif __linux__
				SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
#endif

				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0x00, 0xFF);
				for (int i = 0; i < SCREEN_HEIGHT; i += 4)
				{
#ifdef _WIN32
					SDL_RenderPoint(gRenderer, SCREEN_WIDTH / 2, i);
#elif __linux__
					SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH / 2, i);
#endif
				}

				SDL_Rect topLeftViewport;
				topLeftViewport.x = 0;
				topLeftViewport.y = 0;
				topLeftViewport.w = SCREEN_WIDTH / 2;
				topLeftViewport.h = SCREEN_HEIGHT / 2;
#ifdef _WIN32
				SDL_SetRenderViewport(gRenderer, &topLeftViewport);
#elif __linux__
				SDL_RenderSetViewport(gRenderer, &topLeftViewport);
#endif
#ifdef _WIN32
				SDL_RenderTexture(gRenderer, gTexture, NULL, NULL);
#elif __linux__
				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
#endif

				SDL_Rect topRightViewport;
				topRightViewport.x = SCREEN_WIDTH / 2;
				topRightViewport.y = 0;
				topRightViewport.w = SCREEN_WIDTH / 2;
				topRightViewport.h = SCREEN_HEIGHT / 2;
#ifdef _WIN32
				SDL_SetRenderViewport(gRenderer, &topRightViewport);
#elif __linux__
				SDL_RenderSetViewport(gRenderer, &topRightViewport);
#endif
#ifdef _WIN32
				SDL_RenderTexture(gRenderer, gTexture, NULL, NULL);
#elif __linux__
				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
#endif

				SDL_Rect bottomViewport;
				bottomViewport.x = 0;
				bottomViewport.y = SCREEN_HEIGHT / 2;
				bottomViewport.w = SCREEN_WIDTH;
				bottomViewport.h = SCREEN_HEIGHT / 2;
#ifdef _WIN32
				SDL_SetRenderViewport(gRenderer, &bottomViewport);
#elif __linux__
				SDL_RenderSetViewport(gRenderer, &bottomViewport);
#endif
#ifdef _WIN32
				SDL_RenderTexture(gRenderer, gTexture, NULL, NULL);
#elif __linux__
				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);
#endif

#ifdef _WIN32
				SDL_SetRenderViewport(gRenderer, NULL);
#elif __linux__
				SDL_RenderSetViewport(gRenderer, NULL);
#endif
				gBackgroundTexture.render(0, 0);
				gFooTexture.render(240, 190);

				gDotSpriteSheetTexture.render(0, 0, &gDotSpriteClips[0]);
				gDotSpriteSheetTexture.render(SCREEN_WIDTH - gDotSpriteClips[1].w, 0, &gDotSpriteClips[1]);
				gDotSpriteSheetTexture.render(0, SCREEN_HEIGHT - gDotSpriteClips[2].h, &gDotSpriteClips[2]);
				gDotSpriteSheetTexture.render(SCREEN_WIDTH - gDotSpriteClips[3].w, SCREEN_HEIGHT - gDotSpriteClips[3].h, &gDotSpriteClips[3]);

				gBackgroundTexture.render(0, 0);
				gModulatedTexture.setColor(r, g, b);
				gModulatedTexture.setAlpha(a);
				gModulatedTexture.render(0, 0);

#ifdef _WIN32
				SDL_FRect* currentClip = &gWalkingSpriteClips[frame / 4];
#elif __linux__
				SDL_Rect* currentClip = &gWalkingSpriteClips[frame / 4];
#endif
				gWalkingSpriteSheetTexture.render((SCREEN_WIDTH - currentClip->w) / 2, (SCREEN_HEIGHT - currentClip->h) / 2, currentClip);

				gArrowTexture.render((SCREEN_WIDTH - gArrowTexture.getWidth()) / 2, (SCREEN_HEIGHT - gArrowTexture.getHeight()) / 2, NULL, degrees, NULL, flipType);

				gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight()) / 2);

				for (int i = 0; i < TOTAL_BUTTONS; ++i)
				{
					gButtons[i].render();
				}

#ifdef _WIN32
				const bool* currentKeyStates = SDL_GetKeyboardState(NULL);
#elif __linux__
				const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
#endif
				if (currentKeyStates[SDL_SCANCODE_UP])
				{
					currentTexture = &gUpTexture;
				}
				else if (currentKeyStates[SDL_SCANCODE_DOWN])
				{
					currentTexture = &gDownTexture;
				}
				else if (currentKeyStates[SDL_SCANCODE_LEFT])
				{
					currentTexture = &gLeftTexture;
				}
				else if (currentKeyStates[SDL_SCANCODE_RIGHT])
				{
					currentTexture = &gRightTexture;
				}
				else
				{
					currentTexture = &gPressTexture;
				}
				currentTexture->render(0, 0);

				double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);
				//printf("%lf\n", joystickAngle);
				if (xDir == 0 && yDir == 0)
				{
					joystickAngle = 0;
				}
				gArrowTexture.render((SCREEN_WIDTH - gArrowTexture.getWidth()) / 2, (SCREEN_HEIGHT - gArrowTexture.getHeight()) / 2, NULL, joystickAngle);

				//gSplashTexture.render(0, 0);

				SDL_RenderPresent(gRenderer);

				++frame;
				if (frame / 4 >= WALKING_ANIMATION_FRAMES)
				{
					frame = 0;
				}
			}
		}
	}
	
	close();
	delete timer;
    delete clock;
    return 0;
}
