#include "SoftfootFalls.h"

#include "Time.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>

#include "Point.h"


int main(int argc, char* argv[])
{
	if (argc < 1) return 1;
	const int ROW_SIZE = 61;
	const int GRID_SIZE = ROW_SIZE * ROW_SIZE;
	int framesPerSecond = 0;
	Sint32 data[FileIO::TOTAL_DATA]{};
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
	int alphabetIndex = 0;
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
	FileIO::Open(load->Path("nums.bin"), data);
	if (!window.Init())
	{
		SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Renderer* linuxRenderer = GetLinuxRenderer(window.GetWindow());
	if (linuxRenderer) window.SetRenderer(linuxRenderer);
	LoadArt(load);
	Texture* box = &boxes[BoxUp];
	for (int i = 0; i < miracleStarfallLimit; ++i)
	{
		int j = i % 5;
		if (j == 0) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_000_1024x1024.png"));
		else if (j == 1) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_001_1024x1024.png"));
		else if (j == 2) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_002_1024x1024.png"));
		else if (j == 3) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_003_1024x1024.png"));
		else if (j == 4) miracleStarfalls[i].Init(window.GetRenderer(), load->Path("MiracleStarfall_004_1024x1024.png"));
	}
	SDL_Event event;
	int countedFrames = 0;
	Player player;
	int backgroundOffset = 0;
	int currentData = 0;
	bool isHome = false;
	long long previousTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	bool trisecondToggle = false;
	bool sixsecondToggle = false;
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
	while (window.IsRunning())
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
			sixsecondToggle = !sixsecondToggle;
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
		while (SDL_PollEvent(&event) != 0)
		{
			if (IsWindowQuit(event)) window.Quit();
			else if (event.type == KEY_RELEASED)
			{
				switch (Key(event))
				{
				case SDLK_HOME:
					break;
				case KEY_W:
					player.SetUp(false);
					isUp = false;
					break;
				case KEY_S:
					player.SetDown(false);
					isDown = false;
					break;
				case KEY_A:
					player.SetLeft(false);
					isLeft = false;
					break;
				case KEY_D:
					player.SetRight(false);
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
			else if (event.type == KEY_PRESSED)
			{
				isIdle = false;
				switch (Key(event))
				{
				case SDLK_HOME:
					isHome = !isHome;
					break;
				case KEY_W:
					box = &boxes[BoxUp];
					--currentData;
					if (currentData < 0) currentData = FileIO::TOTAL_DATA - 1;
					player.SetUp(true);
					isUp = true;
					break;
				case KEY_S:
					box = &boxes[BoxDown];
					++currentData;
					if (currentData == FileIO::TOTAL_DATA) currentData = 0;
					player.SetDown(true);
					isDown = true;
					break;
				case KEY_A:
					box = &boxes[BoxLeft];
					--data[currentData];
					player.SetLeft(true);
					isLeft = true;
					break;
				case KEY_D:
					box = &boxes[BoxRight];
					++data[currentData];
					player.SetRight(true);
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
			window.HandleEvent(event);
		}
		if (!window.IsMinimized())
		{
			SDL_RenderClear(window.GetRenderer());
			backgroundOffset = -unsorted[BackgroundBackground].GetHeight() * normals[minute_1];
			if (backgroundOffset < -unsorted[BackgroundBackground].GetHeight()) backgroundOffset = 0;
			DrawBackgroundLeftTop(viewports, backgroundOffset);
			DrawBackgroundLeftMiddle(viewports, backgroundOffset);
			DrawBackgroundLeftBottom(viewports, backgroundOffset);
			DrawBackgroundRightTop(viewports, backgroundOffset);
			DrawBackgroundRightMiddle(viewports, backgroundOffset);
			DrawBackgroundRightBottom(viewports, backgroundOffset);
			int horizontalModifier = 0;
			if (sixsecondToggle) horizontalModifier = (window.GetWidth() + window.GetWidth()) * normals[second_6];
			else horizontalModifier = (window.GetWidth() + window.GetWidth()) * (1.0f - normals[second_6]);
			DrawBossMoonboy(viewports, horizontalModifier);
			DrawBackgroundForeground(viewports);
			DrawBossMoonboyHands(viewports, horizontalModifier);
			player.Move(window.GetWidth(), window.GetHeight(), currentTime, limits[second_3]);
			player.SetIJ(&dotI, &dotJ, &dotNormalI, &dotNormalJ, (float)window.GetWidth(), (float)window.GetHeight(), ROW_SIZE);
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
				SetRow(viewports);
				viewports[v_tile].w = centerX / ROW_SIZE;
				viewports[v_tile].h = centerY / ROW_SIZE;
				viewports[v_tile].x = (window.GetWidth() - viewports[v_tile].w - ((0 + centerX) * normal));
				viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
				SetColumn(viewports);
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
						DrawPlayerHighlight(viewports);
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
					if (i == ROW_SIZE - 1 && dotJ == j) DrawFarTopTile(viewports);
					if (isOnFire[k]) fires[animationIndex].Draw(&viewports[v_tile]);
					if (j == 0) SetFarLeftTile(viewports);
					if (i == dotI) unsorted[PalaceHighlightLeft].Draw(&viewports[v_farLeftTile]);
					SetPlayerBox(viewports, ROW_SIZE, dotNormalJ);
					SetPlayerMiracle(viewports, ROW_SIZE, dotNormalJ);
					SetPlayerClouds(viewports, ROW_SIZE, dotNormalJ);
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
					if (i == 24 && j == 0) unsorted[CharacterFairySun].Draw(&viewports[v_tile], 360 * normals[second_1]);
					if (i == 24 && j == 1) unsorted[CharacterFairyHopeful].Draw(&viewports[v_tile]);
					if (alphabetIndex++ >= AlphabetEnum_Size) alphabetIndex = 0;
					int hallI = 12;
					int hallJ = 11;
					if (i == hallI && j == hallJ) unsorted[Landscape_Hall].Draw(&viewports[v_tile]);
					if (hallI == dotI && hallJ == dotJ) isOverHall = true;
					else isOverHall = false;
					SetTileMiracle(viewports);
					if (i == 0) DrawAlphabet(viewports, j);
					if (i == 2 && j == 10) DrawHouseWithWind(viewports, windIndex, windAngle, isLeft, fireIndex);
					if (i == 1 && j == 11) DrawHouseWithWind(viewports, windIndex, windAngle, isDown, fireIndex);
					if (i == 2 && j == 12) DrawHouseWithWind(viewports, windIndex, windAngle, isRight, fireIndex);
					if (i == 5 && j == 11) DrawHouseWithWind(viewports, windIndex, windAngle, isUp, fireIndex);
					if (i == 2 && j == 17) DrawHouseWithWind(viewports, windIndex, windAngle, isY, fireIndex);
					if (i == 1 && j == 18) DrawHouseWithWind(viewports, windIndex, windAngle, isB, fireIndex);
					if (i == 2 && j == 19) DrawHouseWithWind(viewports, windIndex, windAngle, isA, fireIndex);
					if (i == 5 && j == 18) DrawHouseWithWind(viewports, windIndex, windAngle, isX, fireIndex);
					if (i == 2 && j == 14) DrawHouseWithWind(viewports, windIndex, windAngle, isSelect, fireIndex);
					if (i == 2 && j == 15) DrawHouseWithWind(viewports, windIndex, windAngle, isStart, fireIndex);
					if (i == 9 && j == 10) DrawHouseWithWind(viewports, windIndex, windAngle, isLeftBumper, fireIndex);
					if (i == 9 && j == 19) DrawHouseWithWind(viewports, windIndex, windAngle, isRightBumper, fireIndex);
					if (i == ROW_SIZE - 1 && j == ROW_SIZE - 1) unsorted[CharacterFairyMoon].Draw(&viewports[v_tile], 360 * normals[second_1]);
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
			SetArrow(viewports, normals);
			unsorted[IconCursor].Draw(&viewports[v_arrow], 0.0, SDL_FLIP_VERTICAL);
			SetPalace(viewports, &player);
			unsorted[Loadstone].Draw(&viewports[v_palace]);
			unsorted[BoxFront].Draw(&viewports[v_palace]);
			if (isOverHall)
			{
				SetRight(viewports, dotJ);
				SetRightDown(viewports, dotJ);
				SetRightDownDown(viewports, dotJ);
				unsorted[PlayerHighlight].Draw(&viewports[v_palaceRight]);
				unsorted[PalaceHighlightTop].Draw(&viewports[v_palaceRightDown]);
				unsorted[PalaceHighlightRight].Draw(&viewports[v_palaceRightDownDown]);
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
					SetPlayerStarCurrent(viewports, gMiracleStarfallViewports, m, gMiracleStarfallNormals);
					characterFairyWistfuls[m].Draw(&viewports[v_playerStarCurrent]);
				}
			}
			SetMenuTop(viewports);
			unsorted[MenuTop].Draw(&viewports[v]);
			if (isIdle) unsorted[Crest].Draw(NULL);
			if (isHome)
			{
				SetMenuBottom(viewports);
				unsorted[MenuBottom].Draw(&viewports[v]);
				SetMenuRightTop(viewports);
				unsorted[MenuRightTop].Draw(&viewports[v]);
				SetMenuRightBottom(viewports);
				unsorted[MenuRightBottom].Draw(&viewports[v]);
				SetMenuLeft(viewports);
				unsorted[MenuLeft].Draw(&viewports[v]);
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
	for (int i = 0; i < miracleStarfallLimit; ++i) miracleStarfalls[i].Free();
	FileIO::Write(load->Path("nums.bin"), data);
	UnloadArt();
	window.Free();
	SDL_Quit();
	delete[] miracleStarfalls;
	delete load;
	delete[] basePath;
	return 0;
}
