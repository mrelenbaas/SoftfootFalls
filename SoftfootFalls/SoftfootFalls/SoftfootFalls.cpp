#include "SoftfootFalls.h"

#include "Time.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>

#include "Point.h"
#include "Load.h"


int main(int argc, char* argv[])
{
	if (argc < 1) return 1;
	using namespace std;
	const int TOTAL_DATA = 10;
	Window window;
	Sint32 data[TOTAL_DATA]{};
	const int ROW_SIZE = 25;
	const int GRID_SIZE = ROW_SIZE * ROW_SIZE;
	int framesPerSecond = 0;
	const char* basePath = BasePath(argv[0]);
	Load* load = new Load(basePath);
	long long deltas[TimerEnum_Size]{};
	for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] = 0L;
	double normals[TimerEnum_Size]{};
	for (int i = 0; i < TimerEnum_Size; ++i) normals[i] = 0.0;
	SDL_Rect viewports[ViewportsEnum_Size]{};
	for (int i = 0; i < ViewportsEnum_Size; ++i) viewports[i] = { 0, 0, 0, 0 };
	BoxesEnum playerDirection = BoxUp;
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
	bool isOverHall = false;
	double windAngle = 30.0;
	const int FIRE_SIZE = 3;
	Texture* miracleStarfalls;
	int miracleStarfallIndex = 0;
	int miracleStarfallModIndex = 0;
	const int miracleStarfallLimit = 5;
	bool miracleStarfallUpdateAtEndOfFrame = false;
	miracleStarfalls = new Texture[miracleStarfallLimit]{};
	SDL_Rect gMiracleStarfallViewports[miracleStarfallLimit]{};
	for (int i = 0; i < miracleStarfallLimit; ++i)
	{
		gMiracleStarfallViewports[i].x = 0;
		gMiracleStarfallViewports[i].y = 0;
		gMiracleStarfallViewports[i].w = 0;
		gMiracleStarfallViewports[i].h = 0;
	}
	bool gMiracleStarfallSpawns[miracleStarfallLimit]{};
	for (int i = 0; i < miracleStarfallLimit; ++i) gMiracleStarfallSpawns[i] = false;
	bool gMiracleStarfallDespawns[miracleStarfallLimit]{};
	for (int i = 0; i < miracleStarfallLimit; ++i) gMiracleStarfallDespawns[i] = false;
	long long gMiracleStarfallDeltas[miracleStarfallLimit]{};
	for (int i = 0; i < miracleStarfallLimit; ++i) gMiracleStarfallDeltas[i] = 0L;
	double gMiracleStarfallNormals[miracleStarfallLimit]{};
	for (int i = 0; i < miracleStarfallLimit; ++i) gMiracleStarfallNormals[i] = 0;
	auto file = GetFile();
	file = IOFromFile(load->Path("nums.bin"), "r+b");
	if (file == NULL)
	{
		SDL_Log("Warning: Unable to open file! SDL Error: %s\n", SDL_GetError());
		file = IOFromFile(load->Path("nums.bin"), "w+b");
		if (file != NULL)
		{
			SDL_Log("New file created!\n");
			for (int i = 0; i < TOTAL_DATA; ++i)
			{
				data[i] = 0;
				WriteIO(file, &data[i]);
			}
			CloseIO(file);
		}
		else
		{
			SDL_Log("Error: Unable to create file! SDL Error: %s\n", SDL_GetError());
			return 1;
		}
	}
	else
	{
		SDL_Log("Reading file...!\n");
		for (int i = 0; i < TOTAL_DATA; ++i) ReadIO(file, &data[i]);
		CloseIO(file);
	}
	if (!window.Init())
	{
		SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	else
	{
		SDL_Renderer* linuxRenderer = GetLinuxRenderer(window.GetWindow());
		if (linuxRenderer) window.SetRenderer(linuxRenderer);
	}
	Texture textures[PathEnum_Size];
	Texture horizons[HorizonsEnum_Size];
	Texture roads[RoadsEnum_Size];
	Texture redFire[RedFireEnum_Size];
	Texture blueFire[BlueFireEnum_Size];
	Texture fires[FiresEnum_Size];
	Texture lightnings[LightningEnum_Size];
	Texture clouds[CloudsEnum_Size];
	Texture rainClouds[RainCloudsEnum_Size];
	Texture rains[RainEnum_Size];
	Texture winds[WindEnum_Size];
	Texture houseDenPillars[HouseDenPillarsEnum_Size];
	Texture houseHaunts[HouseHauntsEnum_Size];
	Texture characterFairyWistfuls[CharacterFairyWistfulsEnum_Size];
	Texture alphabet[CharacterFairyWistfulsEnum_Size];
	Texture boxes[BoxesEnum_Size];
	Texture* box = &boxes[BoxUp];
	for (int i = 0; i < PathEnum_Size; ++i) textures[i].Init(window.GetRenderer(), load->Path(Paths[i]));
	for (int i = 0; i < RoadsEnum_Size; ++i) roads[i].Init(window.GetRenderer(), load->Path(RoadsPaths[i]));
	for (int i = 0; i < HorizonsEnum_Size; ++i) horizons[i].Init(window.GetRenderer(), load->Path(HorizonsPaths[i]));
	for (int i = 0; i < RedFireEnum_Size; ++i) redFire[i].Init(window.GetRenderer(), load->Path(redFirePaths[i]));
	for (int i = 0; i < BlueFireEnum_Size; ++i) blueFire[i].Init(window.GetRenderer(), load->Path(blueFirePaths[i]));
	for (int i = 0; i < FiresEnum_Size; ++i) fires[i].Init(window.GetRenderer(), load->Path(firesPaths[i]));
	for (int i = 0; i < LightningEnum_Size; ++i) lightnings[i].Init(window.GetRenderer(), load->Path(lightningsPaths[i]));
	for (int i = 0; i < CloudsEnum_Size; ++i) clouds[i].Init(window.GetRenderer(), load->Path(cloudsPaths[i]));
	for (int i = 0; i < RainCloudsEnum_Size; ++i) rainClouds[i].Init(window.GetRenderer(), load->Path(rainCloudsPaths[i]));
	for (int i = 0; i < RainEnum_Size; ++i) rains[i].Init(window.GetRenderer(), load->Path(rainPaths[i]));
	for (int i = 0; i < WindEnum_Size; ++i) winds[i].Init(window.GetRenderer(), load->Path(windPaths[i]));
	for (int i = 0; i < HouseDenPillarsEnum_Size; ++i) houseDenPillars[i].Init(window.GetRenderer(), load->Path(houseDenPillarsPaths[i]));
	for (int i = 0; i < HouseHauntsEnum_Size; ++i) houseHaunts[i].Init(window.GetRenderer(), load->Path(houseHauntsPaths[i]));
	for (int i = 0; i < BoxesEnum_Size; ++i) boxes[i].Init(window.GetRenderer(), load->Path(boxesPaths[i]));
	for (int i = 0; i < miracleStarfallLimit; ++i)
	{
		int j = i % 5;
		if (j == 0) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_000_1024x1024.png"));
		else if (j == 1) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_001_1024x1024.png"));
		else if (j == 2) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_002_1024x1024.png"));
		else if (j == 3) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_003_1024x1024.png"));
		else if (j == 4) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_004_1024x1024.png"));
	}
	for (int i = 0; i < CharacterFairyWistfulsEnum_Size; ++i) characterFairyWistfuls[i].Init(window.GetRenderer(), load->Path(CharacterFairyWistfulsPaths[i]));
	for (int i = 0; i < CharacterFairyWistfulsEnum_Size; ++i) characterFairyWistfuls[i].Init(window.GetRenderer(), load->Path(CharacterFairyWistfulsPaths[i]));
	bool quit = false;
	SDL_Event e;
	int countedFrames = 0;
	Dot dot;
	int backgroundScrollingOffset = 0;
	int currentData = 0;
	bool isHome = false;
	long long previousTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	bool trisecondToggle = false;
	bool twelvesecondToggle = false;
	int dotI = 0;
	int dotJ = 0;
	float dotNormalI = 0.0f;
	float dotNormalJ = 0.0f;
	int animationIndex = 0;
	int fireIndex = 0;
	bool isLightningActive = false;
	bool isRainActive = false;
	bool isStarActive = false;
	SDL_Rect starBox = { 0, 0, 0, 0 };
	SDL_Rect windBox = { 0, 0, 0, 0 };
	int starI = 0;
	int starJ = 0;
	bool isOnFire[GRID_SIZE]{};
	for (int i = 0; i < GRID_SIZE; ++i) isOnFire[i] = false;
	bool isIdle = true;
	int lightningIndex = 0;
	int cloudIndex = 0;
	int rainCloudIndex = 0;
	int rainIndex = 0;
	int windIndex = 0;
	while (!quit)
	{
		long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		deltas[second_1] += currentTime - previousTime;
		deltas[animation] += currentTime - previousTime;
		deltas[second_3] += currentTime - previousTime;
		deltas[second_6] += currentTime - previousTime;
		deltas[minute_1] += currentTime - previousTime;
		for (int m = 0; m < miracleStarfallLimit; ++m) gMiracleStarfallDeltas[m] += currentTime - previousTime;
		if (deltas[animation] > limits[animation])
		{
			deltas[animation] -= limits[animation];
			++animationIndex;
			if (animationIndex >= ANIMATION_LIMIT) animationIndex = 0;
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
		else ++framesPerSecond;
		if (deltas[second_3] > limits[second_3])
		{
			trisecondToggle = !trisecondToggle;
			deltas[second_3] -= limits[second_3];
		}
		if (deltas[second_6] > limits[second_6])
		{
			twelvesecondToggle = !twelvesecondToggle;
			deltas[second_6] -= limits[second_6];
		}
		if (deltas[minute_1] > limits[minute_1]) deltas[minute_1] -= limits[minute_1];
		for (int i = 0; i < TimerEnum_Size; ++i) normals[i] = (double)deltas[i] / limits[i];
		for (int m = 0; m < miracleStarfallLimit; ++m)
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
			else if (e.type == KEY_RELEASED)
			{
				switch (Key(e))
				{
				case SDLK_HOME:
					break;
				case KEY_W:
					dot.SetUp(false);
					isUp = false;
					break;
				case KEY_S:
					dot.SetDown(false);
					isDown = false;
					break;
				case KEY_A:
					dot.SetLeft(false);
					isLeft = false;
					break;
				case KEY_D:
					dot.SetRight(false);
					isRight = false;
					break;
				case KEY_Q:
					isY = false;
					break;
				case KEY_E:
					isX = false;
					windAngle = 30.0;
					break;
				case KEY_Z:
					isB = false;
					isLightningActive = false;
					break;
				case KEY_C:
					isA = false;
					isRainActive = false;
					break;
				case KEY_X:
					isLeftBumper = false;
					isStarActive = false;
					break;
				}
			}
			else if (e.type == KEY_PRESSED)
			{
				isIdle = false;
				switch (Key(e))
				{
				case SDLK_HOME:
					isHome = !isHome;
					break;
				case KEY_W:
					box = &boxes[BoxUp];
					--currentData;
					if (currentData < 0) currentData = TOTAL_DATA - 1;
					dot.SetUp(true);
					isUp = true;
					break;
				case KEY_S:
					box = &boxes[BoxDown];
					++currentData;
					if (currentData == TOTAL_DATA) currentData = 0;
					dot.SetDown(true);
					isDown = true;
					break;
				case KEY_A:
					box = &boxes[BoxLeft];
					--data[currentData];
					dot.SetLeft(true);
					isLeft = true;
					break;
				case KEY_D:
					box = &boxes[BoxRight];
					++data[currentData];
					dot.SetRight(true);
					isRight = true;
					break;
				case KEY_Q:
					PlaySFX(load->Path("medium.wav"));
					miracleStarfallUpdateAtEndOfFrame = true;
					miracleStarfallModIndex = miracleStarfallIndex % 5;
					gMiracleStarfallSpawns[miracleStarfallIndex] = true;
					gMiracleStarfallDeltas[miracleStarfallIndex] = 0L;
					gMiracleStarfallNormals[miracleStarfallIndex] = (double)gMiracleStarfallDeltas[miracleStarfallIndex] / limits[second_1];
					isY = true;
					break;
				case KEY_E:
					isX = true;
					windAngle = 0.0;
					break;
				case KEY_Z:
					isLightningActive = true;
					isB = true;
					break;
				case KEY_C:
					isRainActive = true;
					isA = true;
					break;
				case KEY_X:
					isLeftBumper = true;
					isStarActive = true;
					break;
				default:
					break;
				}
			}
			window.HandleEvent(e);
		}
		if (!window.IsMinimized())
		{
			SDL_RenderClear(window.GetRenderer());
			switch (playerDirection)
			{
			case BoxUp:
				backgroundScrollingOffset = -textures[BackgroundBackground].GetHeight() * normals[minute_1];
				if (backgroundScrollingOffset < -textures[BackgroundBackground].GetHeight()) backgroundScrollingOffset = 0;
				break;
			case BoxDown:
				backgroundScrollingOffset = textures[BackgroundBackground].GetHeight() * normals[minute_1];
				if (backgroundScrollingOffset > textures[BackgroundBackground].GetHeight()) backgroundScrollingOffset = 0;
				break;
			case BoxLeft:
				backgroundScrollingOffset = -textures[BackgroundBackground].GetWidth() * normals[minute_1];
				if (backgroundScrollingOffset < -textures[BackgroundBackground].GetWidth()) backgroundScrollingOffset = 0;
				break;
			case BoxRight:
				backgroundScrollingOffset = textures[BackgroundBackground].GetWidth() * normals[minute_1];
				if (backgroundScrollingOffset > textures[BackgroundBackground].GetWidth()) backgroundScrollingOffset = 0;
				break;
			case BoxesEnum_Size:
				break;
			}
			viewports[v_background].w = textures[BackgroundBackground].GetWidth();
			viewports[v_background].h = textures[BackgroundBackground].GetHeight();
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
			viewports[v_fullscreen].x = 0;
			viewports[v_fullscreen].y = 0;
			viewports[v_fullscreen].w = window.GetWidth();
			viewports[v_fullscreen].h = window.GetHeight();
			int horizontalModifier = 0;
			if (twelvesecondToggle) horizontalModifier = (window.GetWidth() + viewports[v_fullscreen].w) * normals[second_6];
			else horizontalModifier = (window.GetWidth() + viewports[v_fullscreen].w) * (1.0f - normals[second_6]);
			viewports[v_boss].x = viewports[v_fullscreen].x - viewports[v_fullscreen].w + horizontalModifier;
			viewports[v_boss].y = viewports[v_fullscreen].y - (viewports[v_fullscreen].h / 2);
			viewports[v_boss].w = viewports[v_fullscreen].w;
			viewports[v_boss].h = viewports[v_fullscreen].h;
			textures[CharacterTownspersonMoonboy].Draw(&viewports[v_boss]);
			textures[BackgroundForeground].Draw(&viewports[v_fullscreen]);
			textures[CharacterTownspersonMoonboy_Hands].Draw(&viewports[v_boss]);
			dot.Move(window.GetWidth(), window.GetHeight(), currentTime, limits[second_3]);
			dot.SetIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ, (float)window.GetWidth(), (float)window.GetHeight(), ROW_SIZE);
			viewports[v_tile].x = (window.GetWidth() * 0.5f) - 100;
			viewports[v_tile].y = (window.GetHeight() * 0.5f) - 100;
			viewports[v_tile].w = 200;
			viewports[v_tile].h = 200;
			float normal = normals[second_1];
			float centerX = window.GetWidth() * 0.15f;
			float centerY = window.GetHeight() * 0.5f;
			int horizonI = 0;
			int roadI = 0;
			int k = 0;
			for (int i = ROW_SIZE - 1; i >= 0 ; --i)
			{
				normal = (float)i / (float)ROW_SIZE;
				viewports[v_tile].w = centerX / ROW_SIZE;
				viewports[v_tile].h = centerY / ROW_SIZE;
				viewports[v_tile].x = ((0 + centerX) * normal);
				viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
				viewports[v_row] = viewports[v_tile];
				viewports[v_tile].w = centerX / ROW_SIZE;
				viewports[v_tile].h = centerY / ROW_SIZE;
				viewports[v_tile].x = (window.GetWidth() - viewports[v_tile].w - ((0 + centerX) * normal));
				viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
				viewports[v_column] = viewports[v_tile];
				for (int j = 0; j < ROW_SIZE; ++j)
				{
					float xStep = (float)(viewports[v_column].x + viewports[v_column].w - viewports[v_row].x) / ROW_SIZE;
					viewports[v_tile].w = (viewports[v_column].x + viewports[v_column].w - viewports[v_row].x) / ROW_SIZE;
					viewports[v_tile].h = viewports[v_tile].w;
					viewports[v_tile].x = (viewports[v_row].x + (j * xStep));
					viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
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
					if (horizonI > 3) horizonI = 0;
					modI = (i % 4 == 0);
					roads[modI + roadI].Draw(&viewports[v_tile]);
					++roadI;
					if (roadI > 3) roadI = 0;
					if (dotI == i && dotJ == j)
					{
						if (isLightningActive) isOnFire[k] = true;
						else if (isRainActive) isOnFire[k] = false;
						viewports[v_player] = viewports[v_tile];
						textures[PlayerHighlight].Draw(&viewports[v_player]);
					}
					if (isStarActive)
					{
						starBox.x = (viewports[v_tile].x + window.GetWidth()) - (window.GetWidth() * normals[second_3]);
						starBox.y = (viewports[v_tile].y - window.GetHeight()) + (window.GetHeight() * normals[second_3]);
						starBox.w = viewports[v_tile].w;
						starBox.h = viewports[v_tile].h;
						starI = dotI;
						starJ = dotJ;
					}
					double altWindAngle = windAngle;
					if (windAngle == 0.0)
					{
						windBox.x = (viewports[v_tileMiracle].x + window.GetWidth()) - (window.GetWidth() * normals[second_3]);
						windBox.y = viewports[v_tileMiracle].y;
						windBox.w = viewports[v_tileMiracle].w;
						windBox.h = viewports[v_tileMiracle].h;
						altWindAngle = 125.0;
					}
					miracleStarfalls[0].Draw(&starBox, 360 * normals[second_3]);
					clouds[cloudIndex].Draw(&windBox);
					winds[windIndex].Draw(&windBox, altWindAngle, SDL_FLIP_HORIZONTAL);
					if (i == 24 && dotJ == j)
					{
						viewports[v_farTopTile].x = viewports[v_tile].x;
						viewports[v_farTopTile].y = viewports[v_tile].y;
						viewports[v_farTopTile].w = viewports[v_tile].w;
						viewports[v_farTopTile].h = viewports[v_tile].h;
						textures[PalaceHighlightBottom].Draw(&viewports[v_farTopTile]);
					}
					if (isOnFire[k]) fires[animationIndex].Draw(&viewports[v_tile]);
					if (j == 0)
					{
						viewports[v_farLeftTile].x = viewports[v_tile].x;
						viewports[v_farLeftTile].y = viewports[v_tile].y;
						viewports[v_farLeftTile].w = viewports[v_tile].w;
						viewports[v_farLeftTile].h = viewports[v_tile].h;
					}
					if (i == dotI) textures[PalaceHighlightLeft].Draw(&viewports[v_farLeftTile]);
					viewports[v_playerBox].x = (viewports[v_farLeftTile].x + ((viewports[v_farLeftTile].w * ROW_SIZE) * dotNormalJ) - (viewports[v_farLeftTile].w));
					viewports[v_playerMiracle].x = (viewports[v_farLeftTile].x + ((viewports[v_farLeftTile].w * ROW_SIZE) * dotNormalJ) - (viewports[v_farLeftTile].w));
					viewports[v_playerClouds].x = (viewports[v_farLeftTile].x + ((viewports[v_farLeftTile].w * ROW_SIZE) * dotNormalJ) - (viewports[v_farLeftTile].w)) - (viewports[v_playerClouds].w * 0.1f);
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
					if (i == 24 && j == 1) textures[CharacterFairyHopeful].Draw(&viewports[v_tile]);
					int hallI = 12;
					int hallJ = 11;
					if (i == hallI && j == hallJ) textures[Landscape_Hall].Draw(&viewports[v_tile]);
					if (hallI == dotI && hallJ == dotJ) isOverHall = true;
					else isOverHall = false;
					viewports[v_tileMiracle].x = viewports[v_tile].x;
					viewports[v_tileMiracle].y = viewports[v_tile].y - viewports[v_tile].h;
					viewports[v_tileMiracle].w = viewports[v_tile].w;
					viewports[v_tileMiracle].h = viewports[v_tile].h;
					if (i == 2 && j == 10)
					{
						houseDenPillars[HouseDenPillar_000].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isLeft)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 1 && j == 11)
					{
						houseDenPillars[HouseDenPillar_001].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isDown)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 2 && j == 12)
					{
						houseDenPillars[HouseDenPillar_002].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isRight)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 5 && j == 11)
					{
						houseDenPillars[HouseDenPillar_003].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isUp)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 2 && j == 17)
					{
						houseDenPillars[HouseDenPillar_004].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isY)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 1 && j == 18)
					{
						houseDenPillars[HouseDenPillar_005].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isB)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 2 && j == 19)
					{
						houseDenPillars[HouseDenPillar_006].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isA)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 5 && j == 18)
					{
						houseDenPillars[HouseDenPillar_007].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isX)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 2 && j == 14)
					{
						houseDenPillars[HouseDenPillar_008].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isSelect)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 2 && j == 15)
					{
						houseDenPillars[HouseDenPillar_009].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isStart)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 9 && j == 10)
					{
						houseDenPillars[HouseDenPillar_010].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isLeftBumper)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == 9 && j == 19)
					{
						houseDenPillars[HouseDenPillar_011].Draw(&viewports[v_tile]);
						winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
						(isRightBumper)
							? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
							: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
					}
					if (i == ROW_SIZE - 1 && j == ROW_SIZE - 1) textures[CharacterFairyMoon].Draw(&viewports[v_tile], 360 * normals[second_1]);
					if (dotI == i)
					{
						if (isLightningActive) lightnings[lightningIndex].Draw(&viewports[v_playerMiracle]);
						box->Draw(&viewports[v_playerBox]);
						if (isRainActive) rains[rainIndex].Draw(&viewports[v_playerMiracle]);
						if (isLightningActive || isRainActive) rainClouds[rainCloudIndex].Draw(&viewports[v_playerClouds]);
					}
					++k;
				}
			}
			viewports[v_playerStarTileToGrid].x = ((float)viewports[v_farTopTile].x < (float)viewports[v_player].x)
				? (float)viewports[v_farTopTile].x + (((float)viewports[v_player].x - (float)viewports[v_farTopTile].x) * (1.0f - normals[second_1]))
				: (float)viewports[v_player].x + (((float)viewports[v_farTopTile].x - (float)viewports[v_player].x) * normals[second_1]);
			viewports[v_playerStarTileToGrid].y = (float)viewports[v_farTopTile].y + (((float)viewports[v_player].y - (float)viewports[v_farTopTile].y) * (1.0f - normals[second_1]));
			viewports[v_playerStarTileToGrid].w = viewports[v_player].w - ((viewports[v_player].w - viewports[v_farTopTile].w) * normals[second_1]);
			viewports[v_playerStarTileToGrid].h = viewports[v_player].h - ((viewports[v_player].h - viewports[v_farTopTile].h) * normals[second_1]);
			textures[IconCursor].Draw(&viewports[v_playerStarTileToGrid], 0.0, SDL_FLIP_VERTICAL);
			viewports[v_palace].x = dot.GetBox().x - (dot.GetBox().w / 4 * 10);
			viewports[v_palace].y = dot.GetBox().y - (dot.GetBox().h / 4 * 10);
			viewports[v_palace].w = dot.GetBox().w * 10;
			viewports[v_palace].h = dot.GetBox().h * 10;
			viewports[v_palaceRight].x = (dotJ < 12) ? viewports[v_palace].x + viewports[v_palace].w : viewports[v_palace].x - viewports[v_palace].w;
			viewports[v_palaceRight].y = viewports[v_palace].y;
			viewports[v_palaceRight].h = viewports[v_palace].w;
			viewports[v_palaceRight].w = viewports[v_palace].h;
			viewports[v_palaceRightDown].x = (dotJ < 12) ? viewports[v_palace].x + viewports[v_palace].w : viewports[v_palace].x - viewports[v_palace].w;
			viewports[v_palaceRightDown].y = viewports[v_palace].y + (viewports[v_palace].h * 0.5f);
			viewports[v_palaceRightDown].h = viewports[v_palace].w;
			viewports[v_palaceRightDown].w = viewports[v_palace].h;
			viewports[v_palaceRightDownDown].x = (dotJ < 12) ? viewports[v_palace].x + viewports[v_palace].w : viewports[v_palace].x - viewports[v_palace].w;
			viewports[v_palaceRightDownDown].y = viewports[v_palace].y + (viewports[v_palace].h * 0.75f);
			viewports[v_palaceRightDownDown].h = viewports[v_palace].w;
			viewports[v_palaceRightDownDown].w = viewports[v_palace].h;
			textures[Loadstone].Draw(&viewports[v_palace]);
			textures[BoxFront].Draw(&viewports[v_palace]);
			if (isOverHall)
			{
				textures[PlayerHighlight].Draw(&viewports[v_palaceRight]);
				textures[PalaceHighlightTop].Draw(&viewports[v_palaceRightDown]);
				textures[PalaceHighlightRight].Draw(&viewports[v_palaceRightDownDown]);
			}
			if (gMiracleStarfallSpawns[miracleStarfallIndex])
			{
				gMiracleStarfallViewports[miracleStarfallIndex].x = viewports[v_palace].x;
				gMiracleStarfallViewports[miracleStarfallIndex].y = viewports[v_palace].y;
				gMiracleStarfallViewports[miracleStarfallIndex].w = viewports[v_palace].w;
				gMiracleStarfallViewports[miracleStarfallIndex].h = viewports[v_palace].h;
				gMiracleStarfallDespawns[miracleStarfallIndex] = true;
				gMiracleStarfallSpawns[miracleStarfallIndex] = false;
			}
			for (int m = 0; m < miracleStarfallLimit; ++m)
			{
				if (gMiracleStarfallDeltas[m] > limits[second_1]) gMiracleStarfallDespawns[m] = false;
				if (gMiracleStarfallDespawns[m])
				{
					viewports[v_playerStarCurrent].x = gMiracleStarfallViewports[m].x;
					viewports[v_playerStarCurrent].y = gMiracleStarfallViewports[m].y + (window.GetHeight() * gMiracleStarfallNormals[m]);
					viewports[v_playerStarCurrent].w = gMiracleStarfallViewports[m].w;
					viewports[v_playerStarCurrent].h = gMiracleStarfallViewports[m].h;
					characterFairyWistfuls[m].Draw(&viewports[v_playerStarCurrent]);
				}
			}
			viewports[v_menu].x = viewports[v_fullscreen].x;
			viewports[v_menu].y = viewports[v_fullscreen].y - (viewports[v_fullscreen].h * 0.4f);
			viewports[v_menu].w = viewports[v_fullscreen].w;
			viewports[v_menu].h = viewports[v_fullscreen].h;
			textures[MenuTop].Draw(&viewports[v_menu]);
			if (isIdle) textures[Crest].Draw(&viewports[v_fullscreen]);
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
			SDL_RenderPresent(window.GetRenderer());
		}
		if (miracleStarfallUpdateAtEndOfFrame)
		{
			++miracleStarfallIndex;
			if (miracleStarfallIndex >= miracleStarfallLimit) miracleStarfallIndex = 0;
			miracleStarfallUpdateAtEndOfFrame = false;
		}
		++countedFrames;
	}
	for (int i = 0; i < PathEnum_Size; ++i) textures[i].Free();
	for (int i = 0; i < HorizonsEnum_Size; ++i) horizons[i].Free();
	for (int i = 0; i < RoadsEnum_Size; ++i) roads[i].Free();
	for (int i = 0; i < RedFireEnum_Size; ++i) redFire[i].Free();
	for (int i = 0; i < BlueFireEnum_Size; ++i) blueFire[i].Free();
	for (int i = 0; i < FiresEnum_Size; ++i) fires[i].Free();
	for (int i = 0; i < LightningEnum_Size; ++i) lightnings[i].Free();
	for (int i = 0; i < CloudsEnum_Size; ++i) clouds[i].Free();
	for (int i = 0; i < RainCloudsEnum_Size; ++i) rainClouds[i].Free();
	for (int i = 0; i < RainEnum_Size; ++i) rains[i].Free();
	for (int i = 0; i < WindEnum_Size; ++i) winds[i].Free();
	for (int i = 0; i < HouseDenPillarsEnum_Size; ++i) houseDenPillars[i].Free();
	for (int i = 0; i < HouseHauntsEnum_Size; ++i) houseHaunts[i].Free();
	for (int i = 0; i < BoxesEnum_Size; ++i) boxes[i].Free();
	for (int i = 0; i < CharacterFairyWistfulsEnum_Size; ++i) characterFairyWistfuls[i].Free();
	for (int i = 0; i < miracleStarfallLimit; ++i) miracleStarfalls[i].Free();
	file = IOFromFile(load->Path("nums.bin"), "w+b");
	if (file != NULL)
	{
		for (int i = 0; i < TOTAL_DATA; ++i) WriteIO(file, &data);
		CloseIO(file);
	}
	else printf("Error: Unable to save file! %s\n", SDL_GetError());
	window.Free();
	SDL_Quit();
	delete[] miracleStarfalls;
	delete load;
	delete[] basePath;
	return 0;
}
