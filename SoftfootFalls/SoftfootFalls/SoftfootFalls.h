#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <fstream>
#include <cmath>
#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
//#include <iostream>
//#include <vector>
//#include <string>
//#include <unistd.h> // For execv
#include <sys/wait.h> // For waitpid
#include <cstring> // For strdup
#endif
#include "SDLInterface.h"
#include "SDLWrapper.h"
#include "Window.h"
#include "Texture.h"
#include "Player.h"
#include "Load.h"


Window window;
const int ROW_SIZE = 26;
const int GRID_SIZE = ROW_SIZE * ROW_SIZE;

long long previousTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
long long currentTime = 0L;
bool minuteToggle = false;
int backgroundOffset = 0;
int animationIndex = 0;

///////////////////////////////////////////////////////////////////////
//  Timer  ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

enum TimerEnum
{
	second_1,
	second_3,
	second_6,
	minute_1,
	animation,
	TimerEnum_Size
};
long long deltas[TimerEnum_Size]{};
double normals[TimerEnum_Size]{};
const double ANIMATION_LIMIT = 12.0;
const double limits[] =
{
	1'000'000'000.0,
	3'000'000'000.0,
	6'000'000'000.0,
	60'000'000'000.0,
	1'000'000'000.0 / ANIMATION_LIMIT
};

///////////////////////////////////////////////////////////////////////
//  PATHS  ////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#ifdef _WIN32
const char SEPARATOR = '\\';
#elif __linux__
const char SEPARATOR = '/';
#endif

static const char* BasePath(const char* filePath)
{
	char* basePath = new char[strlen(filePath) + 1];
	size_t i = 0;
	while (i < strlen(filePath))
	{
		basePath[i] = filePath[i];
		++i;
	}
	--i;
	do basePath[i] = '\0';
	while (--i, filePath[i] != SEPARATOR);
	return basePath;
}

enum PathEnum
{
	BackgroundBackground,
	BackgroundForeground,
	IconCursor,
	PlayerHighlight,
	PalaceHighlightBottom,
	PalaceHighlightTop,
	PalaceHighlightLeft,
	PalaceHighlightRight,
	MenuTop,
	MenuBottom,
	MenuRightTop,
	MenuRightBottom,
	MenuLeft,
	CharacterTownspersonMoonboy,
	CharacterTownspersonMoonboy_Hands,
	CharacterMonsterMouth,
	PlayerBeam,
	PathEnum_Size
};
const char* Paths[] =
{
	"bg.png",
	"Landscape_Moon_3300x2550.png",
	"IconCursor.png",
	"PlayerHighlight_000_1024x1024.png",
	"PalaceHighlightBottom_000_2048x2048.png",
	"PalaceHighlightTop_000_2048x2048.png",
	"PalaceHighlightLeft_000_1024x1024.png",
	"PalaceHighlightRight_000_2048x2048.png",
	"MenuTop_000_2048x2048.png",
	"MenuBottom_000_2048x2048.png",
	"MenuRightTop_000_2048x2048.png",
	"MenuRightBottom_000_2048x2048.png",
	"MenuLeft_000_1024x1024.png",
	"CharacterTownspersonMoonboy_000_1024x1024.png",
	"CharacterTownspersonMoonboy_Hands_000_1024x1024.png",
	"CharacterMonsterMouth_000_1024x1024.png",
	"PlayerBeam_000_2048x2048.png"
};

enum RoadsEnum
{
	Road_000,
	Road_001,
	Road_002,
	Road_003,
	Road_004,
	RoadsEnum_Size
};
const char* RoadsPaths[] =
{
	"Road_000_256x256.png",
	"Road_001_256x256.png",
	"Road_002_256x256.png",
	"Road_003_256x256.png",
	"Road_004_256x256.png"
};

enum AlphabetEnum
{
	ASCII033000,
	ASCII034000,
	ASCII035000,
	ASCII036000,
	ASCII037000,
	ASCII038000,
	ASCII039000,
	ASCII040000,
	ASCII041000,
	ASCII042000,
	ASCII043000,
	ASCII044000,
	ASCII045000,
	ASCII046000,
	ASCII047000,
	ASCII048000,
	ASCII049000,
	ASCII050000,
	ASCII051000,
	ASCII052000,
	ASCII053000,
	ASCII054000,
	ASCII055000,
	ASCII056000,
	ASCII057000,
	ASCII058000,
	ASCII059000,
	ASCII060000,
	ASCII061000,
	ASCII062000,
	ASCII063000,
	ASCII064000,
	ASCII065000,
	ASCII066000,
	ASCII067000,
	ASCII068000,
	ASCII069000,
	ASCII070000,
	ASCII071000,
	ASCII072000,
	ASCII073000,
	ASCII074000,
	ASCII075000,
	ASCII076000,
	ASCII077000,
	ASCII078000,
	ASCII079000,
	ASCII080000,
	ASCII081000,
	ASCII082000,
	ASCII083000,
	ASCII084000,
	ASCII085000,
	ASCII086000,
	ASCII087000,
	ASCII088000,
	ASCII089000,
	ASCII090000,
	ASCII091000,
	ASCII092000,
	ASCII093000,
	ASCII094000,
	ASCII095000,
	ASCII096000,
	ASCII097000,
	ASCII098000,
	ASCII099000,
	ASCII101000,
	ASCII102000,
	ASCII103000,
	ASCII104000,
	ASCII105000,
	ASCII106000,
	ASCII107000,
	ASCII108000,
	ASCII109000,
	ASCII110000,
	ASCII111000,
	ASCII112000,
	ASCII113000,
	ASCII114000,
	ASCII115000,
	ASCII116000,
	ASCII117000,
	ASCII118000,
	ASCII119000,
	ASCII120000,
	ASCII121000,
	ASCII122000,
	ASCII123000,
	ASCII124000,
	ASCII125000,
	ASCII126000,
	AlphabetEnum_Size
};
const char* alphabetPaths[] =
{
	"ASCII033000.png",
	"ASCII034000.png",
	"ASCII035000.png",
	"ASCII036000.png",
	"ASCII037000.png",
	"ASCII038000.png",
	"ASCII039000.png",
	"ASCII040000.png",
	"ASCII041000.png",
	"ASCII042000.png",
	"ASCII043000.png",
	"ASCII044000.png",
	"ASCII045000.png",
	"ASCII046000.png",
	"ASCII047000.png",
	"ASCII048000.png",
	"ASCII049000.png",
	"ASCII050000.png",
	"ASCII051000.png",
	"ASCII052000.png",
	"ASCII053000.png",
	"ASCII054000.png",
	"ASCII055000.png",
	"ASCII056000.png",
	"ASCII057000.png",
	"ASCII058000.png",
	"ASCII059000.png",
	"ASCII060000.png",
	"ASCII061000.png",
	"ASCII062000.png",
	"ASCII063000.png",
	"ASCII064000.png",
	"ASCII065000.png",
	"ASCII066000.png",
	"ASCII067000.png",
	"ASCII068000.png",
	"ASCII069000.png",
	"ASCII070000.png",
	"ASCII071000.png",
	"ASCII072000.png",
	"ASCII073000.png",
	"ASCII074000.png",
	"ASCII075000.png",
	"ASCII076000.png",
	"ASCII077000.png",
	"ASCII078000.png",
	"ASCII079000.png",
	"ASCII080000.png",
	"ASCII081000.png",
	"ASCII082000.png",
	"ASCII083000.png",
	"ASCII084000.png",
	"ASCII085000.png",
	"ASCII086000.png",
	"ASCII087000.png",
	"ASCII088000.png",
	"ASCII089000.png",
	"ASCII090000.png",
	"ASCII091000.png",
	"ASCII092000.png",
	"ASCII093000.png",
	"ASCII094000.png",
	"ASCII095000.png",
	"ASCII096000.png",
	"ASCII097000.png",
	"ASCII098000.png",
	"ASCII099000.png",
	"ASCII100000.png",
	"ASCII101000.png",
	"ASCII102000.png",
	"ASCII103000.png",
	"ASCII104000.png",
	"ASCII105000.png",
	"ASCII106000.png",
	"ASCII107000.png",
	"ASCII108000.png",
	"ASCII109000.png",
	"ASCII110000.png",
	"ASCII111000.png",
	"ASCII112000.png",
	"ASCII113000.png",
	"ASCII114000.png",
	"ASCII115000.png",
	"ASCII116000.png",
	"ASCII117000.png",
	"ASCII118000.png",
	"ASCII119000.png",
	"ASCII120000.png",
	"ASCII121000.png",
	"ASCII122000.png",
	"ASCII123000.png",
	"ASCII124000.png",
	"ASCII125000.png",
	"ASCII126000.png"
};

Texture unsorted[PathEnum_Size];
Texture roads[RoadsEnum_Size];
Texture alphabet[AlphabetEnum_Size];

///////////////////////////////////////////////////////////////////////
//  Viewports  ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

enum ViewportsEnum
{
	v,
	v_player,
	v_playerBox,
	v_playerStarCurrent,
	v_arrow,
	v_playerClouds,
	v_playerMiracle,
	v_row,
	v_column,
	v_tile,
	v_tileMiracle,
	v_farTopTile,
	v_farLeftTile,
	v_palace,
	v_palaceRight,
	v_palaceRightDown,
	v_palaceRightDownDown,
	v_boss,
	ViewportsEnum_Size
};
SDL_Rect viewports[ViewportsEnum_Size]{};

void LoadArt(const char* applicationPath)
{
	const char* BASE_PATH = BasePath(applicationPath);
	Load* load = new Load(BasePath(applicationPath));
	for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] = 0L;
	for (int i = 0; i < TimerEnum_Size; ++i) normals[i] = 0.0;
	for (int i = 0; i < ViewportsEnum_Size; ++i) viewports[i] = { 0, 0, 0, 0 };
	for (int i = 0; i < PathEnum_Size; ++i) unsorted[i].Init(window.GetRenderer(), load->Path(Paths[i]));
	for (int i = 0; i < RoadsEnum_Size; ++i) roads[i].Init(window.GetRenderer(), load->Path(RoadsPaths[i]));
	for (int i = 0; i < AlphabetEnum_Size; ++i) alphabet[i].Init(window.GetRenderer(), load->Path(alphabetPaths[i]));
	delete[] BASE_PATH;
	delete load;
}

void LoadSDL()
{
	window.Init();
	SDL_Renderer* linuxRenderer = GetLinuxRenderer(window.GetWindow());
	if (linuxRenderer) window.SetRenderer(linuxRenderer);
}

void UnloadArt()
{
	for (int i = 0; i < PathEnum_Size; ++i) unsorted[i].Free();
	for (int i = 0; i < RoadsEnum_Size; ++i) roads[i].Free();
	for (int i = 0; i < AlphabetEnum_Size; ++i) alphabet[i].Free();
}

void UnloadSDL()
{
	window.Free();
	SDL_Quit();
}

void SetPalace(SDL_Rect* viewports, Player* player)
{
	viewports[v_palace].x = player->GetBox().x - (player->GetBox().w / 4 * 10);
	viewports[v_palace].y = player->GetBox().y - (player->GetBox().h / 4 * 10);
	viewports[v_palace].w = player->GetBox().w * 10;
	viewports[v_palace].h = player->GetBox().h * 10;
}

void SetColumn(SDL_Rect* viewports)
{
	viewports[v_column] = viewports[v_tile];
}

void SetRow(SDL_Rect* viewports)
{
	viewports[v_row] = viewports[v_tile];
}

void SetPlayerStarCurrent(SDL_Rect* viewports, SDL_Rect* miracleStarfallViewports, int index, double* miracleStarfallNormals)
{
	viewports[v_playerStarCurrent].x = miracleStarfallViewports[index].x;
	viewports[v_playerStarCurrent].y = miracleStarfallViewports[index].y + (window.GetHeight() * miracleStarfallNormals[index]);
	viewports[v_playerStarCurrent].w = miracleStarfallViewports[index].w;
	viewports[v_playerStarCurrent].h = miracleStarfallViewports[index].h;
}

void SetPlayer(SDL_Rect* viewports)
{
	viewports[v_player] = viewports[v_tile];
}

void SetLeftTop(SDL_Rect* viewports, int offset)
{
	viewports[v].w = unsorted[BackgroundBackground].GetWidth();
	viewports[v].h = unsorted[BackgroundBackground].GetHeight();
	viewports[v].x = 0;
	viewports[v].y = offset;
}

void SetLeftMiddle(SDL_Rect* viewports, int offset)
{
	viewports[v].y = offset + unsorted[BackgroundBackground].GetHeight();
}

void SetLeftBottom(SDL_Rect* viewports, int offset)
{
	viewports[v].y = offset + (unsorted[BackgroundBackground].GetHeight() * 2);
}

void SetRightTop(SDL_Rect* viewports, int offset)
{
	viewports[v].x = unsorted[BackgroundBackground].GetWidth();
	viewports[v].y = offset;
}

void SetRightMiddle(SDL_Rect* viewports, int offset)
{
	viewports[v].y = offset + unsorted[BackgroundBackground].GetHeight();
}

void SetRightBottom(SDL_Rect* viewports, int offset)
{
	viewports[v].y = offset + (unsorted[BackgroundBackground].GetHeight() * 2);
}

void DrawBackgroundLeftTop(SDL_Rect* viewports, int offset)
{
	SetLeftTop(viewports, offset);
	unsorted[BackgroundBackground].Draw(&viewports[v]);
}

void DrawBackgroundLeftMiddle(SDL_Rect* viewports, int offset)
{
	SetLeftMiddle(viewports, offset);
	unsorted[BackgroundBackground].Draw(&viewports[v]);
}

void DrawBackgroundLeftBottom(SDL_Rect* viewports, int offset)
{
	SetLeftBottom(viewports, offset);
	unsorted[BackgroundBackground].Draw(&viewports[v]);
}

void DrawBackgroundRightTop(SDL_Rect* viewports, int offset)
{
	SetRightTop(viewports, offset);
	unsorted[BackgroundBackground].Draw(&viewports[v]);
}

void DrawBackgroundRightMiddle(SDL_Rect* viewports, int offset)
{
	SetRightMiddle(viewports, offset);
	unsorted[BackgroundBackground].Draw(&viewports[v]);
}

void DrawBackgroundRightBottom(SDL_Rect* viewports, int offset)
{
	SetRightBottom(viewports, offset);
	unsorted[BackgroundBackground].Draw(&viewports[v]);
}

void DrawBackground(SDL_Rect* viewports)
{
	backgroundOffset = -unsorted[BackgroundBackground].GetHeight() * normals[minute_1];
	if (backgroundOffset < -unsorted[BackgroundBackground].GetHeight()) backgroundOffset = 0;
	DrawBackgroundLeftTop(viewports, backgroundOffset);
	DrawBackgroundLeftMiddle(viewports, backgroundOffset);
	DrawBackgroundLeftBottom(viewports, backgroundOffset);
	DrawBackgroundRightTop(viewports, backgroundOffset);
	DrawBackgroundRightMiddle(viewports, backgroundOffset);
	DrawBackgroundRightBottom(viewports, backgroundOffset);
}

void DrawBackgroundForeground(SDL_Rect* viewports)
{
	unsorted[BackgroundForeground].Draw(NULL);
}

void DrawAlphabet(SDL_Rect* viewports, int j)
{
	alphabet[ASCII065000 + j].Draw(&viewports[v_tileMiracle]);
}

void DrawPlayerHighlight(SDL_Rect* viewports)
{
	SetPlayer(viewports);
	unsorted[PlayerHighlight].Draw(&viewports[v_player]);
}

void UpdateTimer()
{
	currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] += currentTime - previousTime;
	if (deltas[animation] > limits[animation])
	{
		deltas[animation] -= limits[animation];
		if (animationIndex++ >= ANIMATION_LIMIT) animationIndex = 0;
	}
	if (deltas[minute_1] > limits[minute_1]) minuteToggle = !minuteToggle;
	for (int i = 0; i < TimerEnum_Size; ++i)
	{
		if (deltas[i] > limits[i]) deltas[i] -= limits[i];
		normals[i] = (double)deltas[i] / limits[i];
	}
	previousTime = currentTime;
}

int TextToIndex(char letter)
{
	return (int)letter - 33;
}

void DrawShellPrompt(SDL_Rect* viewports, const char* text)
{
	const int COLUMN_LIMIT = 20;
	viewports[v].y = 0;
	viewports[v].w = window.GetWidth() / COLUMN_LIMIT;
	viewports[v].h = viewports[v].w;
	//TextToIndex(narrowText);
	for (int i = 0; text[i] != '\0'; ++i)
	{
		viewports[v].x = (window.GetWidth() / COLUMN_LIMIT) * i;
		if (text[i] == ' ') continue;
		alphabet[TextToIndex(text[i])].Draw(&viewports[v]);
	}
}
