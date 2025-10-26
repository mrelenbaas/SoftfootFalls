#include "SoftfootFalls.h"

#include "Print.h"
#include "Time.h"
#include "Clock.h"
#include "Timer.h"
#include <sstream>
#include <iomanip>

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
#include "Texture.h"

#include "Load.h"
#include "Container.h"


static const char* BasePath(const char* filePath)
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

bool SecondInit();
bool loadMedia(Load*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*);
void close(Load*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*);

Window gWindow;
#ifdef _WIN32
SDL_Gamepad* gGameController;
#elif __linux__
SDL_GameController* gGameController;
#endif
SDL_Joystick* gJoystick = NULL;
SDL_Haptic* gJoyHaptic = NULL;
const int FIRE_SIZE = 3;
const int FIRES_SIZE = 12;
Texture gFires[FIRES_SIZE];
Texture* gMiracleStarfalls;
int gMiracleStarfallIndex = 0;
int gMiracleStarfallModIndex = 0;
const int gMiracleStarfallLimit = 120;
bool gMiracleStarfallUpdateAtEndOfFrame = false;
Sint32 gData[TOTAL_DATA];
BoxesEnum gDirection = BoxUp;

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

bool loadMedia(Load* load, Texture* textures, Texture* horizons, Texture* roads, Texture* redFire, Texture* blueFire, Texture* lightnings, Texture* clouds, Texture* rainClouds, Texture* rains, Texture* winds, Texture* boxes)
{
	using namespace std;
	bool success = true;
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
	for (int i = 0; i < PathEnum_Size; ++i) success = textures[i].Init(gWindow.GetRenderer(), load->Path(Paths[i]));
	for (int i = 0; i < RoadsEnum_Size; ++i) roads[i].Init(gWindow.GetRenderer(), load->Path(RoadsPaths[i]));
	for (int i = 0; i < HorizonsEnum_Size; ++i) horizons[i].Init(gWindow.GetRenderer(), load->Path(HorizonsPaths[i]));
	for (int i = 0; i < RedFireEnum_Size; ++i) redFire[i].Init(gWindow.GetRenderer(), load->Path(redFirePaths[i]));
	for (int i = 0; i < BlueFireEnum_Size; ++i) blueFire[i].Init(gWindow.GetRenderer(), load->Path(blueFirePaths[i]));
	std::string preName = "Fire_00";
	std::string middleName = "_64x64_00";
	std::string postName = ".png";
	int k = 0;
	for (int i = 0; i < FIRE_SIZE; ++i)
	{
		for (int j = 0; j < FIRES_SIZE / FIRE_SIZE; ++j)
		{
			std::string name = preName + std::to_string(i) + middleName + std::to_string(j) + postName;
			success = gFires[k].Init(gWindow.GetRenderer(), load->Path(name.c_str()));
			++k;
		}
	}
	for (int i = 0; i < LightningEnum_Size; ++i) lightnings[i].Init(gWindow.GetRenderer(), load->Path(lightningsPaths[i]));
	for (int i = 0; i < CloudsEnum_Size; ++i) clouds[i].Init(gWindow.GetRenderer(), load->Path(cloudsPaths[i]));
	for (int i = 0; i < RainCloudsEnum_Size; ++i) rainClouds[i].Init(gWindow.GetRenderer(), load->Path(rainCloudsPaths[i]));
	for (int i = 0; i < RainEnum_Size; ++i) rains[i].Init(gWindow.GetRenderer(), load->Path(rainPaths[i]));
	for (int i = 0; i < WindEnum_Size; ++i) winds[i].Init(gWindow.GetRenderer(), load->Path(windPaths[i]));
	for (int i = 0; i < BoxesEnum_Size; ++i) boxes[i].Init(gWindow.GetRenderer(), load->Path(boxesPaths[i]));
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		int j = i % 5;
		if (j == 0)
		{
			success = gMiracleStarfalls[i].Init(gWindow.GetRenderer(), load->Path("MiracleStarfall_000_1024x1024.png"));
		}
		else if (j == 1)
		{
			success = gMiracleStarfalls[i].Init(gWindow.GetRenderer(), load->Path("MiracleStarfall_001_1024x1024.png"));
		}
		else if (j == 2)
		{
			success = gMiracleStarfalls[i].Init(gWindow.GetRenderer(), load->Path("MiracleStarfall_002_1024x1024.png"));
		}
		else if (j == 3)
		{
			success = gMiracleStarfalls[i].Init(gWindow.GetRenderer(), load->Path("MiracleStarfall_003_1024x1024.png"));
		}
		else if (j == 4)
		{
			success = gMiracleStarfalls[i].Init(gWindow.GetRenderer(), load->Path("MiracleStarfall_004_1024x1024.png"));
		}
	}
	return success;
}

void close(Load* load, Texture* textures, Texture* horizons, Texture* roads, Texture* redFire, Texture* blueFire, Texture* lightnings, Texture* clouds, Texture* rainClouds, Texture* rains, Texture* winds, Texture* boxes)
{
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
	gGameController = NULL;
	gJoystick = NULL;
	gJoyHaptic = NULL;
	for (int i = 0; i < PathEnum_Size; ++i) textures[i].Free();
	for (int i = 0; i < HorizonsEnum_Size; ++i) horizons[i].Free();
	for (int i = 0; i < RoadsEnum_Size; ++i) roads[i].Free();
	for (int i = 0; i < RedFireEnum_Size; ++i) redFire[i].Free();
	for (int i = 0; i < BlueFireEnum_Size; ++i) blueFire[i].Free();
	int k = 0;
	for (int i = 0; i < FIRE_SIZE; ++i)
		for (int j = 0; j < FIRES_SIZE / FIRE_SIZE; ++j)
		{
			gFires[k].Free();
			++k;
		}
	for (int i = 0; i < LightningEnum_Size; ++i) lightnings[i].Free();
	for (int i = 0; i < CloudsEnum_Size; ++i) clouds[i].Free();
	for (int i = 0; i < RainCloudsEnum_Size; ++i) rainClouds[i].Free();
	for (int i = 0; i < RainEnum_Size; ++i) rains[i].Free();
	for (int i = 0; i < WindEnum_Size; ++i) winds[i].Free();
	for (int i = 0; i < BoxesEnum_Size; ++i) boxes[i].Free();
	for (int i = 0; i < gMiracleStarfallLimit; ++i) gMiracleStarfalls[i].Free();
	gWindow.Free();
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	const int ROW_SIZE = 25;
	const int GRID_SIZE = ROW_SIZE * ROW_SIZE;
	Clock* clock = new Clock();
	Timer* myTimer = new Timer(Timer::Print, 1);
	const char* basePath = BasePath(argv[0]);
	Load* load = new Load(basePath);

	gMiracleStarfalls = new Texture[gMiracleStarfallLimit]{};
	SDL_Rect gMiracleStarfallViewports[gMiracleStarfallLimit]{};
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallViewports[i].x = 0;
		gMiracleStarfallViewports[i].y = 0;
		gMiracleStarfallViewports[i].w = 0;
		gMiracleStarfallViewports[i].h = 0;
	}
	bool gMiracleStarfallSpawns[gMiracleStarfallLimit]{};
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallSpawns[i] = false;
	}
	bool gMiracleStarfallDespawns[gMiracleStarfallLimit]{};
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallDespawns[i] = false;
	}
	long long gMiracleStarfallDeltas[gMiracleStarfallLimit]{};
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallDeltas[i] = 0L;
	}
	double gMiracleStarfallNormals[gMiracleStarfallLimit]{};
	for (int i = 0; i < gMiracleStarfallLimit; ++i)
	{
		gMiracleStarfallNormals[i] = 0;
	}
	Point point1 = { 0.0f, 0.0f };
	Point point2 = { 0.0f, 0.0f };
	bool isSecondPointReady = false;

	if (!SecondInit())
	{
		SDL_Log("Failed to initialize!\n");
	}
	else
	{
		Texture textures[PathEnum_Size];
		Texture horizons[HorizonsEnum_Size];
		Texture roads[RoadsEnum_Size];
		Texture redFire[RedFireEnum_Size];
		Texture blueFire[BlueFireEnum_Size];
		Texture lightnings[LightningEnum_Size];
		Texture clouds[CloudsEnum_Size];
		Texture rainClouds[RainCloudsEnum_Size];
		Texture rains[RainEnum_Size];
		Texture winds[WindEnum_Size];
		Texture boxes[BoxesEnum_Size];
		Texture* gBox = &boxes[BoxUp];
		if (!loadMedia(load, textures, horizons, roads, redFire, blueFire, lightnings, clouds, rainClouds, rains, winds, boxes))
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

			long long previousTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			long long animationDelta = 0L;
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
			const double ANIMATION_LIMIT = 12.0;
			int animationIndex = 0;
			int fireIndex = 0;
			bool isOnFire[GRID_SIZE]{};
			for (int i = 0; i < GRID_SIZE; ++i)
			{
				isOnFire[i] = false;
			}
			bool isIdle = true;
			int lightningIndex = 0;
			int cloudIndex = 0;
			int rainCloudIndex = 0;
			int rainIndex = 0;
			int windIndex = 0;
			while (!quit)
			{
				long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				animationDelta += currentTime - previousTime;
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

				double decisecondLimit = 100'000'000.0;
				double secondLimit = 1'000'000'000.0;
				double animationLimit = secondLimit / ANIMATION_LIMIT;
				double trisecondLimit = 3'000'000'000.0;
				double twelvesecondLimit = 6'000'000'000.0;
				double minuteLimit = 60'000'000'000.0;
				double hourLimit = 3'600'000'000'000;
				double halfDayLimit = 43'200'000'000'000.0;
				double fullDayLimit = 86'400'000'000'000.0;
				if (decisecondDelta > decisecondLimit)
				{
					decisecondDelta -= decisecondLimit;
					++gridCounter;
					if (gridCounter > ROW_SIZE * ROW_SIZE)
					{
						gridCounter = 0;
					}
				}
				if (animationDelta > animationLimit)
				{
					animationDelta -= animationLimit;
					++animationIndex;
					if (animationIndex >= ANIMATION_LIMIT)
					{
						animationIndex = 0;
					}
					fireIndex = animationIndex % FIRE_SIZE;
					lightningIndex = animationIndex % LightningEnum_Size;
					cloudIndex = animationIndex % CloudsEnum_Size;
					rainCloudIndex = animationIndex % RainCloudsEnum_Size;
					rainIndex = animationIndex % RainEnum_Size;
					windIndex = animationIndex % WindEnum_Size;
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
				double animationNormal = (double)animationDelta / animationLimit;
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

						case SDLK_UP:
						case KEY_W:
							dot.SetUp(false);
							break;
						case SDLK_DOWN:
						case KEY_S:
							dot.SetDown(false);
							break;
						case SDLK_LEFT:
						case KEY_A:
							dot.SetLeft(false);
							break;
						case SDLK_RIGHT:
						case KEY_D:
							dot.SetRight(false);
							break;
						}
					}
					else if (e.type == KEY_PRESSED)
					{
						isIdle = false;
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
							case BoxUp:
								gDirection = BoxDown;
								break;
							case BoxDown:
								gDirection = BoxLeft;
								break;
							case BoxLeft:
								gDirection = BoxRight;
								break;
							case BoxRight:
								gDirection = BoxUp;
								break;
							case BoxesEnum_Size:
								break;
							}
#ifdef _WIN32
#elif __linux__
							system("aplay ~/SoftfootFalls/SoftfootFalls/x64/Debug/art/scratch.wav");
#endif
							break;
						case SDLK_UP:
						case KEY_W:
							gBox = &boxes[BoxUp];
							--currentData;
							if (currentData < 0)
							{
								currentData = TOTAL_DATA - 1;
							}
							++playerI;
							if (playerI >= ROW_SIZE) playerI = ROW_SIZE - 1;
							dot.SetUp(true);
							break;
						case SDLK_DOWN:
						case KEY_S:
							gBox = &boxes[BoxDown];
							++currentData;
							if (currentData == TOTAL_DATA)
							{
								currentData = 0;
							}
							--playerI;
							if (playerI < 0) playerI = 0;
							dot.SetDown(true);
							break;
						case SDLK_LEFT:
						case KEY_A:
							gBox = &boxes[BoxLeft];
							--gData[currentData];
							--playerJ;
							if (playerJ < 0) playerJ = 0;
							degrees -= 60;
							dot.SetLeft(true);
							break;
						case SDLK_RIGHT:
						case KEY_D:
							gBox = &boxes[BoxRight];
							++gData[currentData];
							++playerJ;
							if (playerJ >= ROW_SIZE) playerJ = ROW_SIZE - 1;
							degrees += 60;
							dot.SetRight(true);
							break;
						case KEY_Q:
							gMiracleStarfallUpdateAtEndOfFrame = true;
							gMiracleStarfallModIndex = gMiracleStarfallIndex % 5;
							gMiracleStarfallSpawns[gMiracleStarfallIndex] = true;
							gMiracleStarfallDeltas[gMiracleStarfallIndex] = 0L;
							gMiracleStarfallNormals[gMiracleStarfallIndex] = (double)gMiracleStarfallDeltas[gMiracleStarfallIndex] / secondLimit;
							break;
						case KEY_E:
							point1.x = (float)gWindow.GetWidth();
							point1.y = 0.0f;
							isSecondPointReady = true;
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
					case BoxUp:
						backgroundScrollingOffset = -textures[BackgroundBackground].GetHeight() * minuteNormal;
						if (backgroundScrollingOffset < -textures[BackgroundBackground].GetHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxDown:
						backgroundScrollingOffset = textures[BackgroundBackground].GetHeight() * minuteNormal;
						if (backgroundScrollingOffset > textures[BackgroundBackground].GetHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxLeft:
						backgroundScrollingOffset = -textures[BackgroundBackground].GetWidth() * minuteNormal;
						if (backgroundScrollingOffset < -textures[BackgroundBackground].GetWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxRight:
						backgroundScrollingOffset = textures[BackgroundBackground].GetWidth() * minuteNormal;
						if (backgroundScrollingOffset > textures[BackgroundBackground].GetWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxesEnum_Size:
						break;
					}

					minuteAngle = 360 * minuteNormal;

					SDL_Rect backgroundViewport =
					{
						0,
						0,
						backgroundViewport.w = textures[BackgroundBackground].GetWidth(),
						textures[BackgroundBackground].GetHeight()
					};
					switch (gDirection)
					{
					case BoxUp:
						backgroundViewport.x = 0;
						backgroundViewport.y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = 0;
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = 0;
						backgroundViewport.y = backgroundScrollingOffset + (textures[BackgroundBackground].GetHeight() * 2);
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset + (textures[BackgroundBackground].GetHeight() * 2);
						textures[BackgroundBackground].Draw(&backgroundViewport);
						break;
					case BoxDown:
						backgroundViewport.x = 0;
						backgroundViewport.y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = 0;
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = 0;
						backgroundViewport.y = backgroundScrollingOffset - textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset - textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						break;
					case BoxLeft:
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + (textures[BackgroundBackground].GetWidth() * 2);
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + (textures[BackgroundBackground].GetWidth() * 2);
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						break;
					case BoxRight:
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset - textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset - textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						break;
					case BoxesEnum_Size:
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
					textures[CharacterTownspersonMoonboy].Draw(&characterViewport);
					textures[BackgroundForeground].Draw(&fullscreenViewport);
					textures[CharacterTownspersonMoonboy_Hands].Draw(&characterViewport);
					dot.Move(gWindow.GetWidth(), gWindow.GetHeight(), currentTime, secondLimit);
					dot.SetIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ, (float)gWindow.GetWidth(), (float)gWindow.GetHeight(), ROW_SIZE);

					SDL_Rect middleViewport =
					{
						middleViewport.x = (gWindow.GetWidth() * 0.5f) - 100,
						middleViewport.y = (gWindow.GetHeight() * 0.5f) - 100,
						middleViewport.w = 200,
						middleViewport.h = 200
					};
					SetRenderViewport(gWindow.GetRenderer(), NULL);
					float normal = secondNormal;
					float centerX = gWindow.GetWidth() * 0.15f;
					float centerY = gWindow.GetHeight() * 0.5f;
					int horizonI = 0;
					int roadI = 0;
					int k = 0;
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
							SDL_Rect extraRaisedViewport =
							{
								middleViewport.x - (middleViewport.w * 0.5f),
								middleViewport.y - (middleViewport.h * 0.5f) - (middleViewport.h * 1.25f),
								middleViewport.w * 2.0f,
								middleViewport.h * 2.0f
							};
							SDL_Rect raisedViewport =
							{
								middleViewport.x - (middleViewport.w * 0.5f),
								middleViewport.y - (middleViewport.h * 0.5f),// -(middleViewport.h * 1.25f),
								middleViewport.w * 2.0f,
								middleViewport.h * 2.0f
							};
							SDL_Rect cloudsViewport =
							{
								middleViewport.x - (middleViewport.w * 0.5f),
								middleViewport.y - (middleViewport.h * 1.1f),
								middleViewport.w * 2.0f,
								middleViewport.h * 2.0f
							};
							int modI = (i % 2 == 0) ? 0 : 4;
							horizons[modI + horizonI].Draw(&middleViewport);
							++horizonI;
							if (horizonI > 3)
							{
								horizonI = 0;
							}
							modI = (i % 4 == 0);
							roads[modI + roadI].Draw(&middleViewport);
							++roadI;
							clouds[cloudIndex].Draw(&middleViewport);
							if (roadI > 3)
							{
								roadI = 0;
							}
							if (dotI == i && dotJ == j)
							{
								isOnFire[k] = true;
								textures[PlayerHighlight].Draw(&middleViewport);
							}
							if (isOnFire[k])
							{
								gFires[animationIndex].Draw(&middleViewport);
							}
							SetRenderViewport(gWindow.GetRenderer(), &middleViewport);
							Distance distance = { (float)middleViewport.w, (float)middleViewport.h };
							if (i == 0 && j == 0)
							{
								textures[IconCursor].Draw(&middleViewport);
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
								textures[IconCursor].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 0)
							{
								textures[CharacterFairySun].Draw(&middleViewport, 360 * secondNormal);
							}
							if (i == ROW_SIZE - 1 && j == ROW_SIZE - 1)
							{
								textures[CharacterFairyMoon].Draw(&middleViewport, 360 * secondNormal);
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
								walkingSpriteViewport.x = farLeft.x + ((farLeft.w * ROW_SIZE) * dotNormalJ) - (farLeft.w),
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
								extraRaisedViewport.x = walkingSpriteViewport.x;
								raisedViewport.x = walkingSpriteViewport.x;
								cloudsViewport.x = walkingSpriteViewport.x;
								lightnings[lightningIndex].Draw(&raisedViewport);
								gBox->Draw(&extraRaisedViewport);
								redFire[fireIndex].Draw(&extraRaisedViewport);
								SetRenderViewport(gWindow.GetRenderer(), &cloudsViewport);
								winds[windIndex].Draw(&raisedViewport);
								rainClouds[rainCloudIndex].Draw(&raisedViewport);
								rains[rainIndex].Draw(&raisedViewport);
								SetRenderViewport(gWindow.GetRenderer(), NULL);
								RenderLine(
									gWindow.GetRenderer(),
									dot.GetBox().x + (dot.GetBox().w * 2.5f),
									dot.GetBox().y + (dot.GetBox().h * 2.5f),
									extraRaisedViewport.x + (extraRaisedViewport.w * 0.5f),
									extraRaisedViewport.y + (extraRaisedViewport.h * 0.5f));
							}
							++k;
						}
					}

					Point beamPoint =
					{
						(int)beamViewport.x + ((int)beamViewport.w / 2.0f),
						(int)beamViewport.y + ((int)beamViewport.h / 2.0f)
					};
					Point linePoint =
					{
						(dot.GetBox().x + beamPoint.x) / 2,
						(dot.GetBox().y + beamPoint.y) / 2
					};
					SDL_Rect lineViewport =
					{
						(int)linePoint.x,
						(int)linePoint.y,
						100,
						100
					};
					textures[CharacterFairyHopeful].Draw(&lineViewport);
					SDL_Rect monsterViewport =
					{
						monsterViewport.x = fullscreenViewport.x,
						monsterViewport.y = fullscreenViewport.y - fullscreenViewport.h + verticalModifier,
						monsterViewport.w = fullscreenViewport.w,
						monsterViewport.h = fullscreenViewport.h
					};
					SetRenderViewport(gWindow.GetRenderer(), &monsterViewport);
					SetRenderViewport(gWindow.GetRenderer(), &fullscreenViewport);
					SDL_Rect palaceViewport
					{
						dot.GetBox().x - (dot.GetBox().w / 4 * 10),
						dot.GetBox().y - (dot.GetBox().h / 4 * 10),
						dot.GetBox().w * 10,
						dot.GetBox().h * 10
					};
					textures[BoxFront].Draw(&palaceViewport);
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
							gMiracleStarfalls[m].Draw(&currentStar, 360 * secondNormal);
						}
					}
					if (isSecondPointReady)
					{
						point2.x = beamPoint.x;
						point2.y = beamPoint.y;
						isSecondPointReady = false;
					}
					SDL_Rect townBuilderMiracleStarfallViewport =
					{
						(point1.x + point2.x) * secondNormal,
						(point1.y + point2.y) * secondNormal,
						100,
						100
					};
					SetRenderViewport(gWindow.GetRenderer(), NULL);
					SDL_SetRenderDrawColor(gWindow.GetRenderer(), 0xFF, 0xFF, 0xFF, 0xFF);
					RenderLine(gWindow.GetRenderer(), point1.x, point1.y, point2.x, point2.y);
					RenderLine(gWindow.GetRenderer(), point2.x, point2.y, point2.x + 100, point2.y);
					RenderLine(gWindow.GetRenderer(), point2.x + 100, point2.y, point2.x + 100, point2.y + 100);
					RenderLine(gWindow.GetRenderer(), point2.x + 100, point2.y + 100, point2.x, point2.y + 100);
					RenderLine(gWindow.GetRenderer(), point2.x, point2.y + 100, point2.x, point2.y);
					float palaceHighlightPercent = 0.75f;
					SDL_Rect palaceHighlightBottomViewport =
					{
						palaceViewport.x,
						palaceViewport.y + (palaceViewport.h * palaceHighlightPercent),
						palaceViewport.w,
						palaceViewport.h
					};
					blueFire[fireIndex].Draw(&palaceHighlightBottomViewport);
					textures[PalaceHighlightBottom].Draw(&palaceHighlightBottomViewport);
					SDL_Rect palaceHighlightTopViewport =
					{
						palaceViewport.x,
						palaceViewport.y - (palaceViewport.h * palaceHighlightPercent),
						palaceViewport.w,
						palaceViewport.h
					};
					textures[PalaceHighlightTop].Draw(&palaceHighlightTopViewport);
					SDL_Rect palaceHighlightLeftViewport =
					{
						palaceViewport.x + (palaceViewport.w * palaceHighlightPercent),
						palaceViewport.y,
						palaceViewport.w,
						palaceViewport.h
					};
					textures[PalaceHighlightLeft].Draw(&palaceHighlightLeftViewport);
					SDL_Rect palaceHighlightRightViewport =
					{
						palaceViewport.x - (palaceViewport.w * palaceHighlightPercent),
						palaceViewport.y,
						palaceViewport.w,
						palaceViewport.h
					};
					textures[PalaceHighlightRight].Draw(&palaceHighlightRightViewport);

					SDL_Rect menuTopViewport =
					{
						fullscreenViewport.x,
						fullscreenViewport.y - (fullscreenViewport.h * 0.4f),
						fullscreenViewport.w,
						fullscreenViewport.h
					};
					textures[MenuTop].Draw(&menuTopViewport);

					if (isIdle)
					{
						textures[Crest].Draw(&fullscreenViewport);
					}

					if (isDebug)
					{
						SDL_Rect menuBottomViewport =
						{
							fullscreenViewport.x,
							fullscreenViewport.h - (fullscreenViewport.h * 0.7f),
							fullscreenViewport.w,
							fullscreenViewport.h
						};
						textures[MenuBottom].Draw(&menuBottomViewport);
						SDL_Rect menuRightTopViewport =
						{
							fullscreenViewport.w * 0.5f,
							fullscreenViewport.h - (fullscreenViewport.h * 0.95f),
							fullscreenViewport.w * 0.5f,
							fullscreenViewport.h * 0.6f
						};
						textures[MenuRightTop].Draw(&menuRightTopViewport);
						SDL_Rect menuRightBottomViewport =
						{
							fullscreenViewport.w * 0.5f,
							fullscreenViewport.h - (fullscreenViewport.h * 0.75f),
							fullscreenViewport.w * 0.5f,
							fullscreenViewport.h * 0.6f
						};
						textures[MenuRightBottom].Draw(&menuRightBottomViewport);
						SDL_Rect menuLeftViewport =
						{
							fullscreenViewport.x,
							fullscreenViewport.h - (fullscreenViewport.h * 0.9f),
							fullscreenViewport.w * 0.5f,
							fullscreenViewport.h * 0.6f
						};
						textures[MenuLeft].Draw(&menuLeftViewport);
						SetRenderViewport(gWindow.GetRenderer(), NULL);
					}
					rains[rainIndex].Draw(NULL);
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
		close(load, textures, horizons, roads, redFire, blueFire, lightnings, clouds, rainClouds, rains, winds, boxes);
	}

	delete[] gMiracleStarfalls;

	delete load;
	delete[] basePath;
	delete myTimer;
	delete clock;
	return 0;
}
