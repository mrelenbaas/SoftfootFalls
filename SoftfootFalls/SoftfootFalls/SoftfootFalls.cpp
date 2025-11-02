#include "SoftfootFalls.h"

#include "Print.h"
#include "Time.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>

#include "Point.h"
#include "Load.h"
#include "Container.h"
#include "Star.h"


const int JOYSTICK_DEAD_ZONE = 8000;
const int TOTAL_DATA = 10;
const double PI = 3.14159265358979323846;

bool SecondInit();
bool loadMedia(Load*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*, Texture*);
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
const int gMiracleStarfallLimit = 50;
bool gMiracleStarfallUpdateAtEndOfFrame = false;
Sint32 gData[TOTAL_DATA];

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

bool loadMedia(Load* load, Texture* textures, Texture* horizons, Texture* roads, Texture* redFire, Texture* blueFire, Texture* lightnings, Texture* clouds, Texture* rainClouds, Texture* rains, Texture* winds, Texture* houseDenPillars, Texture* houseHaunts, Texture* boxes)
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
	for (int i = 0; i < HouseDenPillarsEnum_Size; ++i) houseDenPillars[i].Init(gWindow.GetRenderer(), load->Path(houseDenPillarsPaths[i]));
	for (int i = 0; i < HouseHauntsEnum_Size; ++i) houseHaunts[i].Init(gWindow.GetRenderer(), load->Path(houseHauntsPaths[i]));
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
	int framesPerSecond = 0;
	const char* basePath = BasePath(argv[0]);
	Load* load = new Load(basePath);
	long long deltas[TimerEnum_Size]{};
	for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] = 0L;
	double normals[TimerEnum_Size]{};
	for (int i = 0; i < TimerEnum_Size; ++i) normals[i] = 0.0;
	double limits[TimerEnum_Size]{};
	limits[second_1] = 1'000'000'000.0;
	limits[second_3] = 3'000'000'000.0;
	limits[second_12] = 6'000'000'000.0;
	limits[minute_1] = 60'000'000'000.0;
	const double ANIMATION_LIMIT = 12.0;
	limits[animation] = limits[second_1] / ANIMATION_LIMIT;
	SDL_Rect viewports[ViewportsEnum_Size]{};
	for (int i = 0; i < ViewportsEnum_Size; ++i) viewports[i] = { 0, 0, 0, 0 };
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	BoxesEnum playerDirection = BoxUp;
	BoxesEnum backgroundDirection = BoxUp;
	bool isUp = false;
	bool isDown = false;
	bool isLeft = false;
	bool isRight = false;
	bool isA = false;
	bool isB = false;
	bool isX = false;
	bool isY = false;
	bool isLeftBumper = false;
	bool isRightBumper = false;
	bool isStart = false;
	bool isSelect = false;

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
		Texture houseDenPillars[HouseDenPillarsEnum_Size];
		Texture houseHaunts[HouseHauntsEnum_Size];
		Texture boxes[BoxesEnum_Size];
		Texture* box = &boxes[BoxUp];
		if (!loadMedia(load, textures, horizons, roads, redFire, blueFire, lightnings, clouds, rainClouds, rains, winds, houseDenPillars, houseHaunts, boxes))
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
			bool isHome = false;
			bool isInput = false;

			long long previousTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			bool trisecondToggle = false;
			bool twelvesecondToggle = false;
			int gridCounter = 0;
			int dotI = 0;
			int dotJ = 0;
			float dotNormalI = 0.0f;
			float dotNormalJ = 0.0f;
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
				//Uint64 now = SDL_GetPerformanceCounter();
				//double deltaTime = (double)(now - lastTime) / SDL_GetPerformanceFrequency();
				//lastTime = now;
				long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
				deltas[second_1] += currentTime - previousTime;
				deltas[animation] += currentTime - previousTime;
				deltas[second_3] += currentTime - previousTime;
				deltas[second_12] += currentTime - previousTime;
				deltas[minute_1] += currentTime - previousTime;
				for (int m = 0; m < gMiracleStarfallLimit; ++m)
				{
					gMiracleStarfallDeltas[m] += currentTime - previousTime;
				}

				double hourLimit = 3'600'000'000'000;
				double halfDayLimit = 43'200'000'000'000.0;
				double fullDayLimit = 86'400'000'000'000.0;
				if (deltas[animation] > limits[animation])
				{
					deltas[animation] -= limits[animation];
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
				if (deltas[second_1] > limits[second_1])
				{
					printf("%i\n", framesPerSecond);
					framesPerSecond = 0;
					deltas[second_1] -= limits[second_1];
				}
				else
				{
					++framesPerSecond;
				}
				if (deltas[second_3] > limits[second_3])
				{
					trisecondToggle = !trisecondToggle;
					deltas[second_3] -= limits[second_3];
				}
				if (deltas[second_12] > limits[second_12])
				{
					twelvesecondToggle = !twelvesecondToggle;
					deltas[second_12] -= limits[second_12];
				}
				if (deltas[minute_1] > limits[minute_1])
				{
					deltas[minute_1] -= limits[minute_1];
				}
				for (int i = 0; i < TimerEnum_Size; ++i)
				{
					normals[i] = (double)deltas[i] / limits[i];
				}
				for (int m = 0; m < gMiracleStarfallLimit; ++m)
				{
					gMiracleStarfallNormals[m] = (double)gMiracleStarfallDeltas[m] / limits[second_1];
					if (gMiracleStarfallNormals[m] > 1.0)
					{
						gMiracleStarfallDeltas[m] = 0L;
						gMiracleStarfallDespawns[m] = false;
					}
				}
				previousTime = currentTime;

				while (SDL_PollEvent(&e) != 0)
				{
					if (IsWindowQuit(e)) quit = true;
					else if (e.type == EVENT_JOYSTICK_BUTTON_DOWN)
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
								box = &boxes[BoxLeft];
								--gData[currentData];
								degrees -= 60;
								dot.SetLeft(true);
								xDir = -1;
							}
							else
							{
								dot.SetLeft(false);
								xDir = 0;
							}
							if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
							{
								box = &boxes[BoxRight];
								++gData[currentData];
								degrees += 60;
								dot.SetRight(true);
								xDir = 1;
							}
							else
							{
								dot.SetRight(false);
								xDir = 0;
							}
						}
						else if (e.jaxis.axis == 1)
						{
							if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
							{
								box = &boxes[BoxUp];
								--currentData;
								if (currentData < 0)
								{
									currentData = TOTAL_DATA - 1;
								}
								dot.SetUp(true);
								yDir = -1;
							}
							else
							{
								dot.SetUp(false);
								yDir = 0;
							}
							if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
							{
								box = &boxes[BoxDown];
								++currentData;
								if (currentData == TOTAL_DATA)
								{
									currentData = 0;
								}
								dot.SetDown(true);
								yDir = 1;
							}
							else
							{
								dot.SetDown(false);
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
							isUp = false;
							break;
						case SDLK_DOWN:
						case KEY_S:
							dot.SetDown(false);
							isDown = false;
							break;
						case SDLK_LEFT:
						case KEY_A:
							dot.SetLeft(false);
							isLeft = false;
							break;
						case SDLK_RIGHT:
						case KEY_D:
							dot.SetRight(false);
							isRight = false;
							break;
						case KEY_Q:
							isY = false;
							break;
						case KEY_E:
							isX = false;
							break;
						}
					}
					else if (e.type == KEY_PRESSED)
					{
						isIdle = false;
#ifdef _WIN32
						size_t requiredSize;
						mbstowcs_s(&requiredSize, nullptr, 0, load->Path("medium.wav"), _TRUNCATE);
						std::vector<wchar_t> wideBuffer(requiredSize);
						mbstowcs_s(&requiredSize, wideBuffer.data(), requiredSize, load->Path("medium.wav"), _TRUNCATE);
						std::wstring wideString(wideBuffer.data());
#endif
						switch (Key(e))
						{
						case SDLK_HOME:
							isHome = !isHome;
							if (isInput) StopTextInput(gWindow.GetWindow());
							else StartTextInput(gWindow.GetWindow());
							isInput = !isInput;
							break;
						case SDLK_END:
							switch (backgroundDirection)
							{
							case BoxUp:
								backgroundDirection = BoxDown;
								break;
							case BoxDown:
								backgroundDirection = BoxLeft;
								break;
							case BoxLeft:
								backgroundDirection = BoxRight;
								break;
							case BoxRight:
								backgroundDirection = BoxUp;
								break;
							case BoxesEnum_Size:
								break;
							}
#ifdef _WIN32
							PlaySound(wideString.c_str(), NULL, SND_FILENAME | SND_ASYNC);
#elif __linux__
							system("aplay ~/SoftfootFalls/SoftfootFalls/x64/Debug/art/scratch.wav");
#endif
							break;
						case SDLK_UP:
						case KEY_W:
							box = &boxes[BoxUp];
							--currentData;
							if (currentData < 0)
							{
								currentData = TOTAL_DATA - 1;
							}
							dot.SetUp(true);
							isUp = true;
							break;
						case SDLK_DOWN:
						case KEY_S:
							box = &boxes[BoxDown];
							++currentData;
							if (currentData == TOTAL_DATA)
							{
								currentData = 0;
							}
							dot.SetDown(true);
							isDown = true;
							break;
						case SDLK_LEFT:
						case KEY_A:
							box = &boxes[BoxLeft];
							--gData[currentData];
							degrees -= 60;
							dot.SetLeft(true);
							isLeft = true;
							break;
						case SDLK_RIGHT:
						case KEY_D:
							box = &boxes[BoxRight];
							++gData[currentData];
							degrees += 60;
							dot.SetRight(true);
							isRight = true;
							break;
						case KEY_Q:
							gMiracleStarfallUpdateAtEndOfFrame = true;
							gMiracleStarfallModIndex = gMiracleStarfallIndex % 5;
							gMiracleStarfallSpawns[gMiracleStarfallIndex] = true;
							gMiracleStarfallDeltas[gMiracleStarfallIndex] = 0L;
							gMiracleStarfallNormals[gMiracleStarfallIndex] = (double)gMiracleStarfallDeltas[gMiracleStarfallIndex] / limits[second_1];
							isY = true;
							break;
						case KEY_E:
							isX = true;
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

					switch (playerDirection)
					{
					case BoxUp:
						backgroundScrollingOffset = -textures[BackgroundBackground].GetHeight() * normals[minute_1];
						if (backgroundScrollingOffset < -textures[BackgroundBackground].GetHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxDown:
						backgroundScrollingOffset = textures[BackgroundBackground].GetHeight() * normals[minute_1];
						if (backgroundScrollingOffset > textures[BackgroundBackground].GetHeight())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxLeft:
						backgroundScrollingOffset = -textures[BackgroundBackground].GetWidth() * normals[minute_1];
						if (backgroundScrollingOffset < -textures[BackgroundBackground].GetWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxRight:
						backgroundScrollingOffset = textures[BackgroundBackground].GetWidth() * normals[minute_1];
						if (backgroundScrollingOffset > textures[BackgroundBackground].GetWidth())
						{
							backgroundScrollingOffset = 0;
						}
						break;
					case BoxesEnum_Size:
						break;
					}

					minuteAngle = 360 * normals[minute_1];

					viewports[v_background].w = textures[BackgroundBackground].GetWidth();
					viewports[v_background].h = textures[BackgroundBackground].GetHeight();
					switch (playerDirection)
					{
					case BoxUp:
						viewports[v_background].x = 0;
						viewports[v_background].y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset + (textures[BackgroundBackground].GetHeight() * 2);
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = textures[BackgroundBackground].GetWidth();
						viewports[v_background].y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset + (textures[BackgroundBackground].GetHeight() * 2);
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						break;
					case BoxDown:
						viewports[v_background].x = 0;
						viewports[v_background].y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset - textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = textures[BackgroundBackground].GetWidth();
						viewports[v_background].y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].y = backgroundScrollingOffset - textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						break;
					case BoxLeft:
						viewports[v_background].x = backgroundScrollingOffset;
						viewports[v_background].y = 0;
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset + (textures[BackgroundBackground].GetWidth() * 2);
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset;
						viewports[v_background].y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset + (textures[BackgroundBackground].GetWidth() * 2);
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						break;
					case BoxRight:
						viewports[v_background].x = backgroundScrollingOffset;
						viewports[v_background].y = 0;
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset - textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset;
						viewports[v_background].y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						viewports[v_background].x = backgroundScrollingOffset - textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&viewports[v_background]);
						break;
					case BoxesEnum_Size:
						break;
					}
					viewports[v_fullscreen].x = 0;
					viewports[v_fullscreen].y = 0;
					viewports[v_fullscreen].w = gWindow.GetWidth();
					viewports[v_fullscreen].h = gWindow.GetHeight();
					int horizontalModifier = 0;
					int verticalModifier = 0;
					if (twelvesecondToggle)
					{
						horizontalModifier = (gWindow.GetWidth() + viewports[v_fullscreen].w) * normals[second_12];
						verticalModifier = (gWindow.GetHeight() + viewports[v_fullscreen].h) * normals[second_12];
					}
					else
					{
						horizontalModifier = (gWindow.GetWidth() + viewports[v_fullscreen].w) * (1.0f - normals[second_12]);
						verticalModifier = (gWindow.GetHeight() + viewports[v_fullscreen].h) * (1.0f - normals[second_12]);
					}
					viewports[v_boss].x = viewports[v_fullscreen].x - viewports[v_fullscreen].w + horizontalModifier;
					viewports[v_boss].y = viewports[v_fullscreen].y - (viewports[v_fullscreen].h / 2);
					viewports[v_boss].w = viewports[v_fullscreen].w;
					viewports[v_boss].h = viewports[v_fullscreen].h;
					textures[CharacterTownspersonMoonboy].Draw(&viewports[v_boss]);
					textures[BackgroundForeground].Draw(&viewports[v_fullscreen]);
					textures[CharacterTownspersonMoonboy_Hands].Draw(&viewports[v_boss]);
					dot.Move(gWindow.GetWidth(), gWindow.GetHeight(), currentTime, limits[second_3]);
					dot.SetIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ, (float)gWindow.GetWidth(), (float)gWindow.GetHeight(), ROW_SIZE);

					viewports[v_tile].x = (gWindow.GetWidth() * 0.5f) - 100;
					viewports[v_tile].y = (gWindow.GetHeight() * 0.5f) - 100;
					viewports[v_tile].w = 200;
					viewports[v_tile].h = 200;
					float normal = normals[second_1];
					float centerX = gWindow.GetWidth() * 0.15f;
					float centerY = gWindow.GetHeight() * 0.5f;
					int horizonI = 0;
					int roadI = 0;
					int k = 0;
					for (int i = ROW_SIZE - 1; i >= 0 ; --i)
					{
						normal = (float)i / (float)ROW_SIZE;
						x = (0 + centerX) * normal;
						viewports[v_tile].w = centerX / ROW_SIZE;
						viewports[v_tile].h = centerY / ROW_SIZE;
						y = 
							gWindow.GetHeight()
							- viewports[v_tile].h
							- ((0 + centerY) * normal);
						viewports[v_tile].x = x;
						viewports[v_tile].y = y;
						viewports[v_row] = viewports[v_tile];
						x = gWindow.GetWidth() - viewports[v_tile].w - ((0 + centerX) * normal);
						viewports[v_tile].w = centerX / ROW_SIZE;
						viewports[v_tile].h = centerY / ROW_SIZE;
						y = 
							gWindow.GetHeight()
							- viewports[v_tile].h
							- ((0 + centerY) * normal);
						viewports[v_tile].x = x;
						viewports[v_tile].y = y;
						viewports[v_column] = viewports[v_tile];
						for (int j = 0; j < ROW_SIZE; ++j)
						{
							w = viewports[v_column].x + viewports[v_column].w - viewports[v_row].x;
							float xStep = (float)w / ROW_SIZE;
							x = viewports[v_row].x + (j * xStep);
							viewports[v_tile].w = w / ROW_SIZE;
							viewports[v_tile].h = viewports[v_tile].w;
							y = 
								gWindow.GetHeight()
								- viewports[v_tile].h
								- ((0 + centerY) * normal);
							viewports[v_tile].x = x;
							viewports[v_tile].y = y;
							if (viewports[v_tile].x < 0) viewports[v_tile].x = 0;
							if (viewports[v_tile].y < 0) viewports[v_tile].y = 0;
							if (viewports[v_tile].w < 0) viewports[v_tile].w = 0;
							if (viewports[v_tile].h < 0) viewports[v_tile].h = 0;
							++viewports[v_tile].w;
							viewports[v_playerBox].x = viewports[v_tile].x - (viewports[v_tile].w * 0.5f);
							viewports[v_playerBox].y = viewports[v_tile].y - (viewports[v_tile].h * 0.5f) - (viewports[v_tile].h * 1.25f);
							viewports[v_playerBox].w = viewports[v_tile].w * 2.0f;
							viewports[v_playerBox].h = viewports[v_tile].h * 2.0f;
							viewports[v_playerMiracle].x = viewports[v_tile].x - (viewports[v_tile].w * 0.5f);
							viewports[v_playerMiracle].y = viewports[v_tile].y - (viewports[v_tile].h * 0.5f);
							viewports[v_playerMiracle].w = viewports[v_tile].w * 2.0f;
							viewports[v_playerMiracle].h = viewports[v_tile].h * 2.0f;
							viewports[v_playerClouds].x = viewports[v_tile].x - (viewports[v_tile].w * 0.8f);
							viewports[v_playerClouds].y = viewports[v_tile].y - (viewports[v_tile].h * 1.4f);
							viewports[v_playerClouds].w = viewports[v_tile].w * 2.6f;
							viewports[v_playerClouds].h = viewports[v_tile].h * 2.2f;
							int modI = (i % 2 == 0) ? 0 : 4;
							horizons[modI + horizonI].Draw(&viewports[v_tile]);
							++horizonI;
							if (horizonI > 3)
							{
								horizonI = 0;
							}
							modI = (i % 4 == 0);
							roads[modI + roadI].Draw(&viewports[v_tile]);
							++roadI;
							if (roadI > 3)
							{
								roadI = 0;
							}
							if (dotI == i && dotJ == j)
							{
								isOnFire[k] = true;
								viewports[v_player] = viewports[v_tile];
								textures[PlayerHighlight].Draw(&viewports[v_player]);
							}
							if (i == 24 && dotJ == j)
							{
								viewports[v_farTopTile].x = viewports[v_tile].x;
								viewports[v_farTopTile].y = viewports[v_tile].y;
								viewports[v_farTopTile].w = viewports[v_tile].w;
								viewports[v_farTopTile].h = viewports[v_tile].h;
							}
							if (isOnFire[k])
							{
								gFires[animationIndex].Draw(&viewports[v_tile]);
							}
							Distance distance = { (float)viewports[v_tile].w, (float)viewports[v_tile].h };
							if (j == 0)
							{
								viewports[v_farLeftTile].x = viewports[v_tile].x;
								viewports[v_farLeftTile].y = viewports[v_tile].y;
								viewports[v_farLeftTile].w = viewports[v_tile].w;
								viewports[v_farLeftTile].h = viewports[v_tile].h;
							}
							x = viewports[v_farLeftTile].x + ((viewports[v_farLeftTile].w * ROW_SIZE) * dotNormalJ) - (viewports[v_farLeftTile].w);
							viewports[v_playerBox].x = x;
							viewports[v_playerMiracle].x = x;
							viewports[v_playerClouds].x = x - (viewports[v_playerClouds].w * 0.1f);
							if (i == 0 && j == 0) houseHaunts[HouseHaunt_000_Alt_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 1) houseHaunts[HouseHaunt_001_Alt_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 2) houseHaunts[HouseHaunt_002_Alt_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 3) houseHaunts[HouseHaunt_003_Alt_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 4) houseHaunts[HouseHaunt_004_Alt_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 5) houseHaunts[HouseHaunt_000_Alt_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 6) houseHaunts[HouseHaunt_001_Alt_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 7) houseHaunts[HouseHaunt_002_Alt_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 8) houseHaunts[HouseHaunt_003_Alt_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 9) houseHaunts[HouseHaunt_004_Alt_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 10) houseHaunts[HouseHaunt_000_Alt_Half].Draw(&viewports[v_tile]);
							if (i == 0 && j == 11) houseHaunts[HouseHaunt_001_Alt_Half].Draw(&viewports[v_tile]);
							if (i == 0 && j == 12) houseHaunts[HouseHaunt_002_Alt_Half].Draw(&viewports[v_tile]);
							if (i == 0 && j == 13) houseHaunts[HouseHaunt_003_Alt_Half].Draw(&viewports[v_tile]);
							if (i == 0 && j == 14) houseHaunts[HouseHaunt_004_Alt_Half].Draw(&viewports[v_tile]);
							if (i == 0 && j == 15) houseHaunts[HouseHaunt_000_Normal_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 16) houseHaunts[HouseHaunt_001_Normal_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 17) houseHaunts[HouseHaunt_002_Normal_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 18) houseHaunts[HouseHaunt_003_Normal_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 19) houseHaunts[HouseHaunt_004_Normal_Empty].Draw(&viewports[v_tile]);
							if (i == 0 && j == 20) houseHaunts[HouseHaunt_000_Normal_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 21) houseHaunts[HouseHaunt_001_Normal_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 22) houseHaunts[HouseHaunt_002_Normal_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 23) houseHaunts[HouseHaunt_003_Normal_Full].Draw(&viewports[v_tile]);
							if (i == 0 && j == 24) houseHaunts[HouseHaunt_004_Normal_Full].Draw(&viewports[v_tile]);
							if (i == 1 && j == 0) houseHaunts[HouseHaunt_000_Normal_Half].Draw(&viewports[v_tile]);
							if (i == 1 && j == 1) houseHaunts[HouseHaunt_001_Normal_Half].Draw(&viewports[v_tile]);
							if (i == 1 && j == 2) houseHaunts[HouseHaunt_002_Normal_Half].Draw(&viewports[v_tile]);
							if (i == 1 && j == 3) houseHaunts[HouseHaunt_003_Normal_Half].Draw(&viewports[v_tile]);
							if (i == 1 && j == 4) houseHaunts[HouseHaunt_004_Normal_Half].Draw(&viewports[v_tile]);
							if (i == 24 && j == 0) textures[CharacterFairySun].Draw(&viewports[v_tile], 360 * normals[second_1]);
							if (i == 24 && j == 1) clouds[cloudIndex].Draw(&viewports[v_tile]);
							if (i == 24 && j == 2) textures[PlayerHighlight].Draw(&viewports[v_tile]);
							if (i == 24 && j == 3) textures[PalaceHighlightBottom].Draw(&viewports[v_tile]);
							if (i == 24 && j == 4) textures[PalaceHighlightTop].Draw(&viewports[v_tile]);
							if (i == 24 && j == 5) textures[PalaceHighlightLeft].Draw(&viewports[v_tile]);
							if (i == 24 && j == 6) textures[PalaceHighlightRight].Draw(&viewports[v_tile]);
							if (i == 24 && j == 7) textures[Landscape_Hall].Draw(&viewports[v_tile]);
							if (i == 24 && j == 8) textures[CharacterFairyHopeful].Draw(&viewports[v_tile]);

							viewports[v_tileMiracle].x = viewports[v_tile].x;
							viewports[v_tileMiracle].y = viewports[v_tile].y - viewports[v_tile].h;
							viewports[v_tileMiracle].w = viewports[v_tile].w;
							viewports[v_tileMiracle].h = viewports[v_tile].h;

							if (i == 2 && j == 10)
							{
								houseDenPillars[HouseDenPillar_000].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isLeft)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 1 && j == 11)
							{
								houseDenPillars[HouseDenPillar_001].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isDown)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 2 && j == 12)
							{
								houseDenPillars[HouseDenPillar_002].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isRight)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 5 && j == 11)
							{
								houseDenPillars[HouseDenPillar_003].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isUp)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}

							if (i == 2 && j == 17)
							{
								houseDenPillars[HouseDenPillar_004].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isY)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 1 && j == 18)
							{
								houseDenPillars[HouseDenPillar_005].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isB)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 2 && j == 19)
							{
								houseDenPillars[HouseDenPillar_006].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isA)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 5 && j == 18)
							{
								houseDenPillars[HouseDenPillar_007].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isX)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}

							if (i == 2 && j == 14)
							{
								houseDenPillars[HouseDenPillar_008].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isSelect)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 2 && j == 15)
							{
								houseDenPillars[HouseDenPillar_009].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isStart)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}

							if (i == 9 && j == 10)
							{
								houseDenPillars[HouseDenPillar_010].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isLeftBumper)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}
							if (i == 9 && j == 19)
							{
								houseDenPillars[HouseDenPillar_011].Draw(&viewports[v_tile]);
								winds[windIndex].Draw(&viewports[v_tileMiracle], 30.0, SDL_FLIP_HORIZONTAL);
								(isRightBumper)
									? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
									: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
							}

							if (i == ROW_SIZE - 1 && j == ROW_SIZE - 1)
							{
								textures[CharacterFairyMoon].Draw(&viewports[v_tile], 360 * normals[second_1]);
							}
							if (dotI == i)
							{
								lightnings[lightningIndex].Draw(&viewports[v_playerMiracle]);
								box->Draw(&viewports[v_playerBox]);
								rains[rainIndex].Draw(&viewports[v_playerMiracle]);
								rainClouds[rainCloudIndex].Draw(&viewports[v_playerClouds]);
							}
							++k;
						}
					}

					Point point5 =
					{
						viewports[v_player].x,
						viewports[v_player].y
					};
					Point point6 =
					{
						viewports[v_farTopTile].x,
						viewports[v_farTopTile].y
					};
					viewports[v_playerStarTileToGrid].x = (point6.x < point5.x)
						? point6.x + ((point5.x - point6.x) * (1.0f - normals[second_1]))
						: point5.x + ((point6.x - point5.x) * normals[second_1]);
					viewports[v_playerStarTileToGrid].y = point6.y + ((point5.y - point6.y) * (1.0f - normals[second_1]));
					viewports[v_playerStarTileToGrid].w = viewports[v_player].w - ((viewports[v_player].w - viewports[v_farTopTile].w) * normals[second_1]);
					viewports[v_playerStarTileToGrid].h = viewports[v_player].h - ((viewports[v_player].h - viewports[v_farTopTile].h) * normals[second_1]);
					textures[IconCursor].Draw(&viewports[v_playerStarTileToGrid]);

					Point point7 =
					{
						point6.x,
						point6.y
					};
					Point point8 =
					{
						(point6.x < point5.x)
							? point7.x - (point5.x - point6.x)
							: point7.x + (point6.x - point5.x),
						0
					};
					viewports[v_playerStarGridToSky].x = (point8.x < point7.x)
						? point7.x - ((point7.x - point8.x) * normals[second_1])
						: point7.x + ((point8.x - point7.x) * normals[second_1]);
					viewports[v_playerStarGridToSky].y = point8.y + ((point7.y - point8.y) * (1.0f - normals[second_1]));
					viewports[v_playerStarGridToSky].h = 100;
					viewports[v_playerStarGridToSky].w = 100;
					gMiracleStarfalls[0].Draw(&viewports[v_playerStarGridToSky], 360 * normals[second_3]);

					viewports[v_palace].x = dot.GetBox().x - (dot.GetBox().w / 4 * 10);
					viewports[v_palace].y = dot.GetBox().y - (dot.GetBox().h / 4 * 10);
					viewports[v_palace].w = dot.GetBox().w * 10;
					viewports[v_palace].h = dot.GetBox().h * 10;
					textures[Loadstone].Draw(&viewports[v_palace]);
					textures[BoxFront].Draw(&viewports[v_palace]);
					if (gMiracleStarfallSpawns[gMiracleStarfallIndex])
					{
						gMiracleStarfallViewports[gMiracleStarfallIndex].x = viewports[v_palace].x;
						gMiracleStarfallViewports[gMiracleStarfallIndex].y = viewports[v_palace].y;
						gMiracleStarfallViewports[gMiracleStarfallIndex].w = viewports[v_palace].w;
						gMiracleStarfallViewports[gMiracleStarfallIndex].h = viewports[v_palace].h;
						gMiracleStarfallDespawns[gMiracleStarfallIndex] = true;
						gMiracleStarfallSpawns[gMiracleStarfallIndex] = false;
					}
					for (int m = 0; m < gMiracleStarfallLimit; ++m)
					{
						if (gMiracleStarfallDeltas[m] > limits[second_1])
						{
							gMiracleStarfallDespawns[m] = false;
						}
						if (gMiracleStarfallDespawns[m])
						{
							viewports[v_playerStarCurrent].x = gMiracleStarfallViewports[m].x;
							viewports[v_playerStarCurrent].y = gMiracleStarfallViewports[m].y - (gWindow.GetHeight() * gMiracleStarfallNormals[m]);
							viewports[v_playerStarCurrent].w = gMiracleStarfallViewports[m].w;
							viewports[v_playerStarCurrent].h = gMiracleStarfallViewports[m].h;
							gMiracleStarfalls[m].Draw(&viewports[v_playerStarCurrent], 360 * normals[second_1]);
						}
					}

					viewports[v_menu].x = viewports[v_fullscreen].x;
					viewports[v_menu].y = viewports[v_fullscreen].y - (viewports[v_fullscreen].h * 0.4f);
					viewports[v_menu].w = viewports[v_fullscreen].w;
					viewports[v_menu].h = viewports[v_fullscreen].h;
					textures[MenuTop].Draw(&viewports[v_menu]);
					if (isIdle)
					{
						textures[Crest].Draw(&viewports[v_fullscreen]);
					}
					if (isHome)
					{
						viewports[v_menu].x = viewports[v_fullscreen].x;
						viewports[v_menu].y = viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.7f);
						viewports[v_menu].w = viewports[v_fullscreen].w;
						viewports[v_menu].h = viewports[v_fullscreen].h;
						textures[MenuBottom].Draw(&viewports[v_menu]);
						viewports[v_menu].x = viewports[v_fullscreen].w * 0.5f;
						viewports[v_menu].y = viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.95f);
						viewports[v_menu].w = viewports[v_fullscreen].w * 0.5f;
						viewports[v_menu].h = viewports[v_fullscreen].h * 0.6f;
						textures[MenuRightTop].Draw(&viewports[v_menu]);
						viewports[v_menu].x = viewports[v_fullscreen].w * 0.5f,
						viewports[v_menu].y = viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.75f);
						viewports[v_menu].w = viewports[v_fullscreen].w * 0.5f;
						viewports[v_menu].h = viewports[v_fullscreen].h * 0.6f;
						textures[MenuRightBottom].Draw(&viewports[v_menu]);
						viewports[v_menu].x = viewports[v_fullscreen].x;
						viewports[v_menu].y = viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.9f);
						viewports[v_menu].w = viewports[v_fullscreen].w * 0.5f;
						viewports[v_menu].h = viewports[v_fullscreen].h * 0.6f;
						textures[MenuLeft].Draw(&viewports[v_menu]);
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
		close(load, textures, horizons, roads, redFire, blueFire, lightnings, clouds, rainClouds, rains, winds, boxes);
	}

	delete[] gMiracleStarfalls;

	delete load;
	delete[] basePath;
	//delete myTimer;
	return 0;
}
