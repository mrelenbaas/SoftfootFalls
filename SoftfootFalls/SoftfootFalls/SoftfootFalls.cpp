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
	const char* BASE_PATH = BasePath(argv[0]);
	Load* load = new Load(BASE_PATH);
	for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] = 0L;
	double normals[TimerEnum_Size]{};
	for (int i = 0; i < TimerEnum_Size; ++i) normals[i] = 0.0;
	SDL_Rect viewports[ViewportsEnum_Size]{};
	for (int i = 0; i < ViewportsEnum_Size; ++i) viewports[i] = { 0, 0, 0, 0 };
	BoxesEnum playerDirection = BoxUp;
	int alphabetIndex = 0;
	Texture* miracleStarfalls;
	int miracleStarfallIndex = 0;
	int miracleStarfallModIndex = 0;
	bool miracleStarfallUpdateAtEndOfFrame = false;
	miracleStarfalls = new Texture[MIRACLE_STARFALL_LIMIT]{};
	SDL_Rect gMiracleStarfallViewports[MIRACLE_STARFALL_LIMIT]{};
	for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i)
	{
		gMiracleStarfallViewports[i].x = 0;
		gMiracleStarfallViewports[i].y = 0;
		gMiracleStarfallViewports[i].w = 0;
		gMiracleStarfallViewports[i].h = 0;
	}
	bool gMiracleStarfallSpawns[MIRACLE_STARFALL_LIMIT]{};
	for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i) gMiracleStarfallSpawns[i] = false;
	bool gMiracleStarfallDespawns[MIRACLE_STARFALL_LIMIT]{};
	for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i) gMiracleStarfallDespawns[i] = false;
	long long gMiracleStarfallDeltas[MIRACLE_STARFALL_LIMIT]{};
	for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i) gMiracleStarfallDeltas[i] = 0L;
	double gMiracleStarfallNormals[MIRACLE_STARFALL_LIMIT]{};
	for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i) gMiracleStarfallNormals[i] = 0;
	FileIO::Open(load->Path("nums.bin"), data);
	if (!window.Init()) return 1;
	SDL_Renderer* linuxRenderer = GetLinuxRenderer(window.GetWindow());
	if (linuxRenderer) window.SetRenderer(linuxRenderer);
	LoadArt(load);
	Texture* box = &boxes[BoxUp];
	for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i)
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
	bool minuteToggle = false;
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
	bool isAcceptingInput = true;
	const char* temp = "Insert\0";
	char* text = new char[ROW_SIZE];
	int copyI = 0;
	int copyJ = 0;
	for (copyI = 0; temp[copyI] != '\0'; ++copyI)
	{
		text[copyI] = temp[copyI];
	}
	text[copyI] = '\0';
	char* texts[ROW_SIZE]{};
	for (int copyI = 0; copyI < ROW_SIZE; ++copyI)
	{
		texts[copyI] = new char[ROW_SIZE];
		for (copyJ = 0; copyJ < ROW_SIZE - 1; ++copyJ)
		{
			texts[copyI][copyJ] = '\0';
		}
		texts[copyI][ROW_SIZE - 1] = '\0';
	}
	while (window.IsRunning())
	{
		long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] += currentTime - previousTime;
		for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i) gMiracleStarfallDeltas[i] += currentTime - previousTime;
		if (deltas[animation] > limits[animation])
		{
			deltas[animation] -= limits[animation];
			if (animationIndex++ >= ANIMATION_LIMIT) animationIndex = 0;
			fireIndex = animationIndex % FIRE_SIZE;
			rainIndex = animationIndex % RainEnum_Size;
			windIndex = animationIndex % WindEnum_Size;
			cloudIndex = animationIndex % CloudsEnum_Size;
			lightningIndex = animationIndex % LightningEnum_Size;
			rainCloudIndex = animationIndex % RainCloudsEnum_Size;
		}
		if (deltas[second_1] > limits[second_1])
		{
			printf("%i\n", framesPerSecond);
			framesPerSecond = 0;
		}
		else ++framesPerSecond;
		if (deltas[second_3] > limits[second_3]) trisecondToggle = !trisecondToggle;
		if (deltas[minute_1] > limits[minute_1]) minuteToggle = !minuteToggle;
		for (int i = 0; i < TimerEnum_Size; ++i)
		{
			if (deltas[i] > limits[i]) deltas[i] -= limits[i];
			normals[i] = (double)deltas[i] / limits[i];
		} 
		for (int m = 0; m < MIRACLE_STARFALL_LIMIT; ++m)
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
				if (isAcceptingInput)
				{
					//printf("event: %i\n", (int)Key(event));
#ifdef _WIN32
					SDL_Keycode key_code = event.key.key;
					if (key_code >= SDLK_SPACE && key_code <= SDLK_Z)
#elif __linux__
					SDL_Keycode key_code = event.key.keysym.sym;
					if (key_code >= SDLK_SPACE && key_code <= SDLK_z)
#endif
					{
						char ascii_char = static_cast<char>(key_code);
						printf("%c", ascii_char);
						// Now you have the ASCII character
						copyI = 0;
						while (copyI < ROW_SIZE - 2)
						{
							if (text[copyI] == '\0')
							{
								text[copyI + 1] = '\0';
								text[copyI] = ascii_char;
								break;
							}
							++copyI;
						}
					}
					else
					{
						switch (Key(event))
						{
						case SDLK_BACKSPACE:
							copyI = 0;
							while (copyI < ROW_SIZE - 1)
							{
								if (text[copyI] == '\0')
								{
									text[copyI - 1] = '\0';
									break;
								}
								++copyI;
							}
							break;
						case SDLK_RETURN:
							delete[] texts[ROW_SIZE - 1];
							for (int i = ROW_SIZE - 2; i >= 0; --i)
							{
								texts[i + 1] = texts[i];
								for (copyI = 0; texts[i + 1][copyI] != '\0'; ++copyI)
								{
									//texts[0][copyI] = text[copyI];
									texts[i + 1][copyI] = texts[i][copyI];
								}
							}
							texts[0] = new char[ROW_SIZE];
							//copyI = 0;
							for (copyI = 0; text[copyI] != '\0'; ++copyI)
							{
								texts[0][copyI] = text[copyI];
								text[copyI] = '\0';
							}
							texts[0][copyI] = '\0';
							//printf("%s\n", texts[0]);
							//for (int i = 0; i < ROW_SIZE; ++i)
							//{
							//	printf("%s\n", texts[i]);
							//}
#ifdef _WIN32
							WindowsProcess(texts[0]);
#elif __linux__
							LinuxProcess(texts[0]);
#endif
							break;
						case SDLK_INSERT:
							isAcceptingInput = !isAcceptingInput;
							break;
						}
					}
					continue;
				}
				switch (Key(event))
				{
				case SDLK_HOME:
					isHome = !isHome;
					break;
				case SDLK_INSERT:
					isAcceptingInput = !isAcceptingInput;
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
		if (window.IsMinimized()) continue;
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
		if (minuteToggle) horizontalModifier = (window.GetWidth() + window.GetWidth()) * normals[minute_1];
		else horizontalModifier = (window.GetWidth() + window.GetWidth()) * (1.0f - normals[minute_1]);
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
				//horizons[modI + horizonI].Draw(&viewports[v_tile]);
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
				if (i == ROW_SIZE - 1 && j == ROW_SIZE - 1) unsorted[CharacterFairyMoon].Draw(&viewports[v_tile], 360 * normals[minute_1]);
				if (i == ROW_SIZE - 1 && j == 0) unsorted[CharacterFairySun].Draw(&viewports[v_tile], 360 * normals[minute_1]);
				if (i == HALL_I && j == HALL_J) unsorted[Landscape_Hall].Draw(&viewports[v_tile]);
				if (HALL_I == dotI && HALL_J == dotJ) isHoveroverHall = true;
				else isHoveroverHall = false;
				if (/*i == 0 && */j == alphabetIndex)
				{
					if (texts[i][alphabetIndex] == '\0')
					{
						alphabetIndex = 0;
					}
					else
					{
						alphabet[TextToIndex(texts[i][alphabetIndex])].Draw(&viewports[v_tile]);
						++alphabetIndex;
					}
				}
				SetTileMiracle(viewports);
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
		DrawArrow(viewports, normals);
		DrawPalace(viewports, &player);
		DrawBoxFront(viewports, &player);
		if (isHoveroverHall)
		{
			DrawRight(viewports, dotJ);
			DrawRightDown(viewports, dotJ);
			DrawRightDownDown(viewports, dotJ);
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
		for (int m = 0; m < MIRACLE_STARFALL_LIMIT; ++m)
		{
			if (gMiracleStarfallDeltas[m] > limits[second_1]) gMiracleStarfallDespawns[m] = false;
			if (gMiracleStarfallDespawns[m]) DrawEarthquake(viewports, gMiracleStarfallViewports, m, gMiracleStarfallNormals);
		}
		if (isAcceptingInput)
		{
			DrawAltMenuTop(viewports, text);
		}
		else
		{
			DrawMenuTop(viewports);
		}
		if (isIdle) DrawCrest(viewports);
		if (isHome)
		{
			DrawMenuBottom(viewports);
			DrawMenuRightTop(viewports);
			DrawMenuRightBottom(viewports);
			DrawMenuLeft(viewports);
		}
		SDL_RenderPresent(window.GetRenderer());
		if (miracleStarfallUpdateAtEndOfFrame)
		{
			++miracleStarfallIndex;
			if (miracleStarfallIndex >= MIRACLE_STARFALL_LIMIT) miracleStarfallIndex = 0;
			miracleStarfallUpdateAtEndOfFrame = false;
		}
		++countedFrames;
	}
	for (int i = 0; i < ROW_SIZE; ++i)
	{
		delete[] texts[i];
	}
	delete[] text;
	for (int i = 0; i < MIRACLE_STARFALL_LIMIT; ++i) miracleStarfalls[i].Free();
	FileIO::Write(load->Path("nums.bin"), data);
	UnloadArt();
	window.Free();
	SDL_Quit();
	delete[] miracleStarfalls;
	delete load;
	delete[] BASE_PATH;
	return 0;
}
