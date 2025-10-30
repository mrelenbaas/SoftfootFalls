#include "SoftfootFalls.h"

#include "Print.h"
#include "Time.h"
#include "Clock.h"
#include "Timer.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>

#include "Load.h"
#include "Container.h"


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
int gNodeIndex = 0;
const int NODE_LIMIT = 4;
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
	SDL_Rect viewports[TimerEnum_Size]{};
	for (int i = 0; i < TimerEnum_Size; ++i) viewports[i] = { 0, 0, 0, 0 };

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
			SDL_Rect defaultRect = { -1, -1, -1, -1 };
			SDL_Rect beamViewport = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
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
			SDL_Rect secondStarfallViewport = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
			SDL_Rect farViewport = { defaultRect.x, defaultRect.y, defaultRect.w, defaultRect.h };
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
					++gNodeIndex;
					if (gNodeIndex >= NODE_LIMIT)
					{
						gNodeIndex = 0;
					}
					deltas[second_1] -= limits[second_1];
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

				myTimer->Update();
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
								gBox = &boxes[BoxLeft];
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
								gBox = &boxes[BoxRight];
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
								gBox = &boxes[BoxUp];
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
								gBox = &boxes[BoxDown];
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
							PlaySound(wideString.c_str(), NULL, SND_FILENAME | SND_ASYNC);
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
							dot.SetDown(true);
							break;
						case SDLK_LEFT:
						case KEY_A:
							gBox = &boxes[BoxLeft];
							--gData[currentData];
							degrees -= 60;
							dot.SetLeft(true);
							break;
						case SDLK_RIGHT:
						case KEY_D:
							gBox = &boxes[BoxRight];
							++gData[currentData];
							degrees += 60;
							dot.SetRight(true);
							break;
						case KEY_Q:
							gMiracleStarfallUpdateAtEndOfFrame = true;
							gMiracleStarfallModIndex = gMiracleStarfallIndex % 5;
							gMiracleStarfallSpawns[gMiracleStarfallIndex] = true;
							gMiracleStarfallDeltas[gMiracleStarfallIndex] = 0L;
							gMiracleStarfallNormals[gMiracleStarfallIndex] = (double)gMiracleStarfallDeltas[gMiracleStarfallIndex] / limits[second_1];
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
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.y = backgroundScrollingOffset + (textures[BackgroundBackground].GetHeight() * 2);
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.y = backgroundScrollingOffset + (textures[BackgroundBackground].GetHeight() * 2);
						textures[BackgroundBackground].Draw(&backgroundViewport);
						break;
					case BoxDown:
						backgroundViewport.x = 0;
						backgroundViewport.y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.y = backgroundScrollingOffset - textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = textures[BackgroundBackground].GetWidth();
						backgroundViewport.y = backgroundScrollingOffset;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.y = backgroundScrollingOffset + textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.y = backgroundScrollingOffset - textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						break;
					case BoxLeft:
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + (textures[BackgroundBackground].GetWidth() * 2);
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + (textures[BackgroundBackground].GetWidth() * 2);
						textures[BackgroundBackground].Draw(&backgroundViewport);
						break;
					case BoxRight:
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = 0;
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset - textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset;
						backgroundViewport.y = textures[BackgroundBackground].GetHeight();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset + textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&backgroundViewport);
						backgroundViewport.x = backgroundScrollingOffset - textures[BackgroundBackground].GetWidth();
						textures[BackgroundBackground].Draw(&backgroundViewport);
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
					SDL_Rect characterViewport =
					{
						characterViewport.x = viewports[v_fullscreen].x - viewports[v_fullscreen].w + horizontalModifier,
						characterViewport.y = viewports[v_fullscreen].y - (viewports[v_fullscreen].h / 2),
						characterViewport.w = viewports[v_fullscreen].w,
						characterViewport.h = viewports[v_fullscreen].h
					};
					textures[CharacterTownspersonMoonboy].Draw(&characterViewport);
					textures[BackgroundForeground].Draw(&viewports[v_fullscreen]);
					textures[CharacterTownspersonMoonboy_Hands].Draw(&characterViewport);
					dot.Move(gWindow.GetWidth(), gWindow.GetHeight(), currentTime, limits[second_3]);
					dot.SetIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ, (float)gWindow.GetWidth(), (float)gWindow.GetHeight(), ROW_SIZE);

					SDL_Rect middleViewport =
					{
						middleViewport.x = (gWindow.GetWidth() * 0.5f) - 100,
						middleViewport.y = (gWindow.GetHeight() * 0.5f) - 100,
						middleViewport.w = 200,
						middleViewport.h = 200
					};
					float normal = normals[second_1];
					float centerX = gWindow.GetWidth() * 0.15f;
					float centerY = gWindow.GetHeight() * 0.5f;
					int horizonI = 0;
					int roadI = 0;
					int k = 0;
					SDL_Rect dotBox = dot.GetBox();
					SDL_Rect tileViewport = defaultRect;
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
								middleViewport.y - (middleViewport.h * 0.5f),
								middleViewport.w * 2.0f,
								middleViewport.h * 2.0f
							};
							SDL_Rect cloudsViewport =
							{
								middleViewport.x - (middleViewport.w * 0.8f),
								middleViewport.y - (middleViewport.h * 1.4f),
								middleViewport.w * 2.6f,
								middleViewport.h * 2.2f
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
							if (roadI > 3)
							{
								roadI = 0;
							}
							if (dotI == i && dotJ == j)
							{
								isOnFire[k] = true;
								tileViewport = middleViewport;
								textures[PlayerHighlight].Draw(&tileViewport);
							}
							if (i == 24 && dotJ == j)
							{
								farViewport.x = middleViewport.x;
								farViewport.y = middleViewport.y;
								farViewport.w = 100;
								farViewport.h = 100;
							}
							if (isOnFire[k])
							{
								gFires[animationIndex].Draw(&middleViewport);
							}
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
								textures[CharacterFairySun].Draw(&middleViewport, 360 * normals[second_1]);
							}
							if (i == ROW_SIZE - 1 && j == 1)
							{
								blueFire[fireIndex].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 2)
							{
								textures[CharacterFairyHopeful].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 3)
							{
								clouds[cloudIndex].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 4)
							{
								textures[PlayerHighlight].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 5)
							{
								redFire[fireIndex].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 6)
							{
								winds[windIndex].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 7)
							{
								textures[PalaceHighlightBottom].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 8)
							{
								textures[PalaceHighlightTop].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 9)
							{
								textures[PalaceHighlightLeft].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == 10)
							{
								textures[PalaceHighlightRight].Draw(&middleViewport);
							}
							if (i == ROW_SIZE - 1 && j == ROW_SIZE - 1)
							{
								textures[CharacterFairyMoon].Draw(&middleViewport, 360 * normals[second_1]);
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
								cloudsViewport.x = walkingSpriteViewport.x - (cloudsViewport.w * 0.1f);
								lightnings[lightningIndex].Draw(&raisedViewport);
								gBox->Draw(&extraRaisedViewport);
								rains[rainIndex].Draw(&raisedViewport);
								rainClouds[rainCloudIndex].Draw(&cloudsViewport);
								secondStarfallViewport.x = dotBox.x + (dotBox.w * 2.5f);
								secondStarfallViewport.y = dotBox.y + (dotBox.h * 2.5f);
								secondStarfallViewport.w = extraRaisedViewport.w * 0.5f;
								secondStarfallViewport.h = extraRaisedViewport.h * 0.5f;
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
						(dotBox.x + beamPoint.x) / 2,
						(dotBox.y + beamPoint.y) / 2
					};
					SDL_Rect lineViewport =
					{
						(int)linePoint.x,
						(int)linePoint.y,
						100,
						100
					};
					point1.x = (float)gWindow.GetWidth() * 0.59f;
					point1.y = (float)gWindow.GetHeight() * 0.28f;
					point2.x = dot.GetBox().x;
					point2.y = dot.GetBox().y;

					SDL_Rect point2Viewport =
					{
						point2.x,
						point2.y,
						100,
						100
					};
					SDL_Rect townBuilderMiracleStarfallViewport =
					{
						point2.x + ((point1.x - point2.x) * (1.0f - normals[second_1])),
						point1.y + ((point2.y - point1.y) * normals[second_1]),
						100,
						100
					};
					if (gNodeIndex == 0) gMiracleStarfalls[0].Draw(&townBuilderMiracleStarfallViewport, 360 * normals[second_3]);
					SDL_Rect point1Viewport =
					{
						point1.x,
						point1.y,
						100,
						100
					};
					Point point4 =
					{
						tileViewport.x,
						tileViewport.y
					};
					Point point3 =
					{
						point2.x,
						point2.y
					};
					SDL_Rect point3Viewport =
					{
						point3.x,
						point3.y,
						100,
						100
					};
					SDL_Rect point4Viewport =
					{
						point4.x,
						point4.y,
						100,
						100
					};
					SDL_Rect secondViewport =
					{
						(point4.x < point3.x)
							? point4.x + ((point3.x - point4.x) * (1.0f - normals[second_1]))
							: point3.x + ((point4.x - point3.x) * (normals[second_1])),
						(point4.y < point3.y)
							? point4.y + ((point3.y - point4.y) * (1.0f - normals[second_1]))
							: point3.y + ((point4.y - point3.y) * normals[second_1]),
						100,
						100
					};
					if (gNodeIndex == 1) gMiracleStarfalls[0].Draw(&secondViewport, 360 * normals[second_3]);
					SDL_Rect secondStaticViewport =
					{
						point4.x,
						point4.y,
						100,
						100
					};
					RenderLine(gWindow.GetRenderer(), point4.x, point4.y, point4.x + 100, point4.y);
					RenderLine(gWindow.GetRenderer(), point4.x + 100, point4.y, point4.x + 100, point4.y + 100);
					RenderLine(gWindow.GetRenderer(), point4.x + 100, point4.y + 100, point4.x, point4.y + 100);
					RenderLine(gWindow.GetRenderer(), point4.x, point4.y + 100, point4.x, point4.y);
					Point point5 =
					{
						point4.x,
						point4.y
					};
					Point point6 =
					{
						farViewport.x,
						farViewport.y
					};
					SDL_Rect thirdViewport =
					{
						(point6.x < point5.x)
							? point6.x + ((point5.x - point6.x) * (1.0f - normals[second_1]))
							: point5.x + ((point6.x - point5.x) * normals[second_1]),
						point6.y + ((point5.y - point6.y) * (1.0f - normals[second_1])),
						100,
						100
					};
					if (gNodeIndex == 2) gMiracleStarfalls[0].Draw(&thirdViewport, 360 * normals[second_3]);
					SDL_Rect thirdStaticViewport =
					{
						point6.x,
						point6.y,
						100,
						100
					};
					RenderLine(gWindow.GetRenderer(), point6.x, point6.y, point6.x + 100, point6.y);
					RenderLine(gWindow.GetRenderer(), point6.x + 100, point6.y, point6.x + 100, point6.y + 100);
					RenderLine(gWindow.GetRenderer(), point6.x + 100, point6.y + 100, point6.x, point6.y + 100);
					RenderLine(gWindow.GetRenderer(), point6.x, point6.y + 100, point6.x, point6.y);
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
					SDL_Rect fourthViewport =
					{
						(point8.x < point7.x)
							? point7.x - ((point7.x - point8.x) * normals[second_1])
							: point7.x + ((point8.x - point7.x) * normals[second_1]),
						point8.y + ((point7.y - point8.y) * (1.0f - normals[second_1])),
						100,
						100
					};
					if (gNodeIndex == 3) gMiracleStarfalls[0].Draw(&fourthViewport, 360 * normals[second_3]);
					SDL_Rect fourthStaticViewport =
					{
						point8.x,
						point8.y,
						100,
						100
					};
					RenderLine(gWindow.GetRenderer(), point8.x, point8.y, point8.x + 100, point8.y);
					RenderLine(gWindow.GetRenderer(), point8.x + 100, point8.y, point8.x + 100, point8.y + 100);
					RenderLine(gWindow.GetRenderer(), point8.x + 100, point8.y + 100, point8.x, point8.y + 100);
					RenderLine(gWindow.GetRenderer(), point8.x, point8.y + 100, point8.x, point8.y);
					SDL_Rect monsterViewport =
					{
						monsterViewport.x = viewports[v_fullscreen].x,
						monsterViewport.y = viewports[v_fullscreen].y - viewports[v_fullscreen].h + verticalModifier,
						monsterViewport.w = viewports[v_fullscreen].w,
						monsterViewport.h = viewports[v_fullscreen].h
					};
					SDL_Rect palaceViewport
					{
						dotBox.x - (dotBox.w / 4 * 10),
						dotBox.y - (dotBox.h / 4 * 10),
						dotBox.w * 10,
						dotBox.h * 10
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
						if (gMiracleStarfallDeltas[m] > limits[second_1])
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
							gMiracleStarfalls[m].Draw(&currentStar, 360 * normals[second_1]);
						}
					}

					float palaceHighlightPercent = 0.75f;
					SDL_Rect palaceHighlightBottomViewport =
					{
						palaceViewport.x,
						palaceViewport.y + (palaceViewport.h * palaceHighlightPercent),
						palaceViewport.w,
						palaceViewport.h
					};
					SDL_Rect palaceHighlightTopViewport =
					{
						palaceViewport.x,
						palaceViewport.y - (palaceViewport.h * palaceHighlightPercent),
						palaceViewport.w,
						palaceViewport.h
					};
					SDL_Rect palaceHighlightLeftViewport =
					{
						palaceViewport.x + (palaceViewport.w * palaceHighlightPercent),
						palaceViewport.y,
						palaceViewport.w,
						palaceViewport.h
					};
					SDL_Rect palaceHighlightRightViewport =
					{
						palaceViewport.x - (palaceViewport.w * palaceHighlightPercent),
						palaceViewport.y,
						palaceViewport.w,
						palaceViewport.h
					};

					textures[Loadstone].Draw(&viewports[v_fullscreen]);
					SDL_Rect menuTopViewport =
					{
						viewports[v_fullscreen].x,
						viewports[v_fullscreen].y - (viewports[v_fullscreen].h * 0.4f),
						viewports[v_fullscreen].w,
						viewports[v_fullscreen].h
					};
					textures[MenuTop].Draw(&menuTopViewport);

					if (isIdle)
					{
						textures[Crest].Draw(&viewports[v_fullscreen]);
					}

					if (isHome)
					{
						SDL_Rect menuBottomViewport =
						{
							viewports[v_fullscreen].x,
							viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.7f),
							viewports[v_fullscreen].w,
							viewports[v_fullscreen].h
						};
						textures[MenuBottom].Draw(&menuBottomViewport);
						SDL_Rect menuRightTopViewport =
						{
							viewports[v_fullscreen].w * 0.5f,
							viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.95f),
							viewports[v_fullscreen].w * 0.5f,
							viewports[v_fullscreen].h * 0.6f
						};
						textures[MenuRightTop].Draw(&menuRightTopViewport);
						SDL_Rect menuRightBottomViewport =
						{
							viewports[v_fullscreen].w * 0.5f,
							viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.75f),
							viewports[v_fullscreen].w * 0.5f,
							viewports[v_fullscreen].h * 0.6f
						};
						textures[MenuRightBottom].Draw(&menuRightBottomViewport);
						viewports[v_menuLeft].x = viewports[v_fullscreen].x;
						viewports[v_menuLeft].y = viewports[v_fullscreen].h - (viewports[v_fullscreen].h * 0.9f);
						viewports[v_menuLeft].w = viewports[v_fullscreen].w * 0.5f;
						viewports[v_menuLeft].h = viewports[v_fullscreen].h * 0.6f;
						textures[MenuLeft].Draw(&viewports[v_menuLeft]);
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
	delete myTimer;
	delete clock;
	return 0;
}
