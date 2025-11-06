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
const int HALL_I = 12;
const int HALL_J = 11;
Sint32 data[FileIO::TOTAL_DATA]{};
int framesPerSecond = 0;

const int FIRE_SIZE = 3;

const int MIRACLE_STARFALL_LIMIT = 5;

double windAngle = 30.0;

bool isHoveroverHall = false;

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
	Crest,
	Landscape_Hall,
	BackgroundBackground,
	BackgroundForeground,
	Loadstone,
	IconCursor,
	CharacterFairySun,
	CharacterFairyMoon,
	BoxFront,
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
	"Crest_2048x2048_000.png",
	"Landscape_Hall_3300x2550.png",
	"bg.png",
	"Landscape_Moon_3300x2550.png",
	"Loadstone_000_2048x2048.png",
	"IconCursor.png",
	"CharacterFairySun_000_256x256.png",
	"CharacterFairyMoon_000_256x256.png",
	"BoxFront.png",
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

enum HorizonsEnum
{
	Horizon000,
	Horizon001,
	Horizon002,
	Horizon003,
	Horizon004,
	Horizon005,
	Horizon006,
	Horizon007,
	HorizonsEnum_Size
};
const char* HorizonsPaths[] =
{
	"Horizon000.png",
	"Horizon001.png",
	"Horizon002.png",
	"Horizon003.png",
	"Horizon004.png",
	"Horizon005.png",
	"Horizon006.png",
	"Horizon007.png"
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

enum RedFireEnum
{
	RedFire_000,
	RedFire_001,
	RedFire_002,
	RedFireEnum_Size
};
const char* redFirePaths[] =
{
	"Fire_000_512x512.png",
	"Fire_001_512x512.png",
	"Fire_002_512x512.png"
};

enum BlueFireEnum
{
	BlueFire_000,
	BlueFire_001,
	BlueFire_002,
	BlueFireEnum_Size
};
const char* blueFirePaths[] =
{
	"MiracleLightningFire_000_512x512.png",
	"MiracleLightningFire_001_512x512.png",
	"MiracleLightningFire_002_512x512.png"
};

enum LightningEnum
{
	MiracleLightning_000,
	MiracleLightning_001,
	MiracleLightning_002,
	MiracleLightning_003,
	MiracleLightning_004,
	MiracleLightning_005,
	MiracleLightning_006,
	MiracleLightning_007,
	LightningEnum_Size
};
const char* lightningsPaths[] =
{
	"MiracleLightning_000_512x512.png",
	"MiracleLightning_001_512x512.png",
	"MiracleLightning_002_512x512.png",
	"MiracleLightning_003_512x512.png",
	"MiracleLightning_004_512x512.png",
	"MiracleLightning_005_512x512.png",
	"MiracleLightning_006_512x512.png",
	"MiracleLightning_007_512x512.png"
};

enum CloudsEnum
{
	MiracleCloud_000,
	MiracleCloud_001,
	MiracleCloud_002,
	MiracleCloud_003,
	MiracleCloud_004,
	MiracleCloud_005,
	MiracleCloud_006,
	MiracleCloud_007,
	MiracleCloud_008,
	MiracleCloud_009,
	MiracleCloud_010,
	MiracleCloud_011,
	CloudsEnum_Size
};
const char* cloudsPaths[] =
{
	"MiracleCloud_000_1024x1024.png",
	"MiracleCloud_001_1024x1024.png",
	"MiracleCloud_002_1024x1024.png",
	"MiracleCloud_003_1024x1024.png",
	"MiracleCloud_004_1024x1024.png",
	"MiracleCloud_005_1024x1024.png",
	"MiracleCloud_006_1024x1024.png",
	"MiracleCloud_007_1024x1024.png",
	"MiracleCloud_008_1024x1024.png",
	"MiracleCloud_009_1024x1024.png",
	"MiracleCloud_010_1024x1024.png",
	"MiracleCloud_011_1024x1024.png"
};

enum RainCloudsEnum
{
	MiracleRainCloud_000,
	MiracleRainCloud_001,
	RainCloudsEnum_Size
};
const char* rainCloudsPaths[] =
{
	"MiracleRainCloud_000_1024x1024.png",
	"MiracleRainCloud_001_1024x1024.png"
};

enum RainEnum
{
	MiracleRain_000,
	MiracleRain_001,
	MiracleRain_002,
	RainEnum_Size
};
const char* rainPaths[] =
{
	"MiracleRain_000_1024x1024.png",
	"MiracleRain_001_1024x1024.png",
	"MiracleRain_002_1024x1024.png"
};

enum WindEnum
{
	MiracleStarfallStreamer_000,
	MiracleStarfallStreamer_001,
	MiracleStarfallStreamer_002,
	MiracleStarfallStreamer_003,
	MiracleStarfallStreamer_004,
	WindEnum_Size
};
const char* windPaths[] =
{
	"MiracleStarfallStreamer_000_1024x1024.png",
	"MiracleStarfallStreamer_001_1024x1024.png",
	"MiracleStarfallStreamer_002_1024x1024.png",
	"MiracleStarfallStreamer_003_1024x1024.png",
	"MiracleStarfallStreamer_004_1024x1024.png"
};

enum BoxesEnum
{
	BoxUp,
	BoxDown,
	BoxLeft,
	BoxRight,
	BoxesEnum_Size
};
const char* boxesPaths[] =
{
	"BoxUp.png",
	"BoxDown.png",
	"BoxLeft.png",
	"BoxRight.png"
};

enum FiresEnum
{
	Fire_000_000,
	Fire_000_001,
	Fire_000_002,
	Fire_000_003,
	Fire_000_004,
	Fire_001_000,
	Fire_001_001,
	Fire_001_002,
	Fire_001_003,
	Fire_001_004,
	Fire_002_000,
	Fire_002_001,
	Fire_002_002,
	Fire_002_003,
	Fire_002_004,
	FiresEnum_Size
};
const char* firesPaths[] =
{
	"Fire_000_64x64_000.png",
	"Fire_000_64x64_001.png",
	"Fire_000_64x64_002.png",
	"Fire_000_64x64_003.png",
	"Fire_000_64x64_004.png",
	"Fire_001_64x64_000.png",
	"Fire_001_64x64_001.png",
	"Fire_001_64x64_002.png",
	"Fire_001_64x64_003.png",
	"Fire_001_64x64_004.png",
	"Fire_002_64x64_000.png",
	"Fire_002_64x64_001.png",
	"Fire_002_64x64_002.png",
	"Fire_002_64x64_003.png",
	"Fire_002_64x64_004.png",
};

enum HouseDenPillarsEnum
{
	HouseDenPillar_000,
	HouseDenPillar_001,
	HouseDenPillar_002,
	HouseDenPillar_003,
	HouseDenPillar_004,
	HouseDenPillar_005,
	HouseDenPillar_006,
	HouseDenPillar_007,
	HouseDenPillar_008,
	HouseDenPillar_009,
	HouseDenPillar_010,
	HouseDenPillar_011,
	HouseDenPillarsEnum_Size
};
const char* houseDenPillarsPaths[] =
{
	"HouseDenPillar_000.png",
	"HouseDenPillar_001.png",
	"HouseDenPillar_002.png",
	"HouseDenPillar_003.png",
	"HouseDenPillar_004.png",
	"HouseDenPillar_005.png",
	"HouseDenPillar_006.png",
	"HouseDenPillar_007.png",
	"HouseDenPillar_008.png",
	"HouseDenPillar_009.png",
	"HouseDenPillar_010.png",
	"HouseDenPillar_011.png",
	"HouseDenPillar_012.png"
};

enum HouseHauntsEnum
{
	HouseHaunt_000_Alt_Empty,
	HouseHaunt_001_Alt_Empty,
	HouseHaunt_002_Alt_Empty,
	HouseHaunt_003_Alt_Empty,
	HouseHaunt_004_Alt_Empty,
	HouseHaunt_000_Alt_Full,
	HouseHaunt_001_Alt_Full,
	HouseHaunt_002_Alt_Full,
	HouseHaunt_003_Alt_Full,
	HouseHaunt_004_Alt_Full,
	HouseHaunt_000_Alt_Half,
	HouseHaunt_001_Alt_Half,
	HouseHaunt_002_Alt_Half,
	HouseHaunt_003_Alt_Half,
	HouseHaunt_004_Alt_Half,
	HouseHaunt_000_Normal_Empty,
	HouseHaunt_001_Normal_Empty,
	HouseHaunt_002_Normal_Empty,
	HouseHaunt_003_Normal_Empty,
	HouseHaunt_004_Normal_Empty,
	HouseHaunt_000_Normal_Full,
	HouseHaunt_001_Normal_Full,
	HouseHaunt_002_Normal_Full,
	HouseHaunt_003_Normal_Full,
	HouseHaunt_004_Normal_Full,
	HouseHaunt_000_Normal_Half,
	HouseHaunt_001_Normal_Half,
	HouseHaunt_002_Normal_Half,
	HouseHaunt_003_Normal_Half,
	HouseHaunt_004_Normal_Half,
	HouseHauntsEnum_Size
};
const char* houseHauntsPaths[] =
{
	"HouseHaunt_000_Alt_Empty_256x256.png",
	"HouseHaunt_001_Alt_Empty_256x256.png",
	"HouseHaunt_002_Alt_Empty_256x256.png",
	"HouseHaunt_003_Alt_Empty_256x256.png",
	"HouseHaunt_004_Alt_Empty_256x256.png",
	"HouseHaunt_000_Alt_Full_256x256.png",
	"HouseHaunt_001_Alt_Full_256x256.png",
	"HouseHaunt_002_Alt_Full_256x256.png",
	"HouseHaunt_003_Alt_Full_256x256.png",
	"HouseHaunt_004_Alt_Full_256x256.png",
	"HouseHaunt_000_Alt_Half_256x256.png",
	"HouseHaunt_001_Alt_Half_256x256.png",
	"HouseHaunt_002_Alt_Half_256x256.png",
	"HouseHaunt_003_Alt_Half_256x256.png",
	"HouseHaunt_004_Alt_Half_256x256.png",
	"HouseHaunt_000_Normal_Empty_256x256.png",
	"HouseHaunt_001_Normal_Empty_256x256.png",
	"HouseHaunt_002_Normal_Empty_256x256.png",
	"HouseHaunt_003_Normal_Empty_256x256.png",
	"HouseHaunt_004_Normal_Empty_256x256.png",
	"HouseHaunt_000_Normal_Full_256x256.png",
	"HouseHaunt_001_Normal_Full_256x256.png",
	"HouseHaunt_002_Normal_Full_256x256.png",
	"HouseHaunt_003_Normal_Full_256x256.png",
	"HouseHaunt_004_Normal_Full_256x256.png",
	"HouseHaunt_000_Normal_Half_256x256.png",
	"HouseHaunt_001_Normal_Half_256x256.png",
	"HouseHaunt_002_Normal_Half_256x256.png",
	"HouseHaunt_003_Normal_Half_256x256.png",
	"HouseHaunt_004_Normal_Half_256x256.png"
};

enum CharacterFairyWistfulsEnum
{
	CharacterFairyWistful_000,
	CharacterFairyWistful_001,
	CharacterFairyWistful_002,
	CharacterFairyWistful_003,
	CharacterFairyWistful_004,
	CharacterFairyWistfulsEnum_Size
};
const char* CharacterFairyWistfulsPaths[] =
{
	"CharacterFairyWistful_000_64x64.png",
	"CharacterFairyWistful_001_64x64.png",
	"CharacterFairyWistful_002_64x64.png",
	"CharacterFairyWistful_003_64x64.png",
	"CharacterFairyWistful_004_64x64.png"
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
Texture alphabet[AlphabetEnum_Size];
Texture boxes[BoxesEnum_Size];

void LoadArt(Load* load)
{
	for (int i = 0; i < PathEnum_Size; ++i) unsorted[i].Init(window.GetRenderer(), load->Path(Paths[i]));
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
	for (int i = 0; i < CharacterFairyWistfulsEnum_Size; ++i) characterFairyWistfuls[i].Init(window.GetRenderer(), load->Path(CharacterFairyWistfulsPaths[i]));
	for (int i = 0; i < AlphabetEnum_Size; ++i) alphabet[i].Init(window.GetRenderer(), load->Path(alphabetPaths[i]));
}

void UnloadArt()
{
	for (int i = 0; i < PathEnum_Size; ++i) unsorted[i].Free();
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
	for (int i = 0; i < AlphabetEnum_Size; ++i) alphabet[i].Free();
}

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

void SetX(SDL_Rect* viewports)
{
}

void SetBoss(SDL_Rect* viewports, int horizontalModifier)
{
	viewports[v_boss].x = 0 - window.GetWidth() + horizontalModifier;
	viewports[v_boss].y = 0 - (window.GetHeight() / 2);
	viewports[v_boss].w = window.GetWidth();
	viewports[v_boss].h = window.GetHeight();
}

void SetRightDownDown(SDL_Rect* viewports, int dotJ)
{
	viewports[v_palaceRightDownDown].x = (dotJ < 12) ? viewports[v_palace].x + viewports[v_palace].w : viewports[v_palace].x - viewports[v_palace].w;
	viewports[v_palaceRightDownDown].y = viewports[v_palace].y + (viewports[v_palace].h * 0.75f);
	viewports[v_palaceRightDownDown].h = viewports[v_palace].w;
	viewports[v_palaceRightDownDown].w = viewports[v_palace].h;
}

void SetRightDown(SDL_Rect* viewports, int dotJ)
{
	viewports[v_palaceRightDown].x = (dotJ < 12) ? viewports[v_palace].x + viewports[v_palace].w : viewports[v_palace].x - viewports[v_palace].w;
	viewports[v_palaceRightDown].y = viewports[v_palace].y + (viewports[v_palace].h * 0.5f);
	viewports[v_palaceRightDown].h = viewports[v_palace].w;
	viewports[v_palaceRightDown].w = viewports[v_palace].h;
}

void SetRight(SDL_Rect* viewports, int dotJ)
{
	viewports[v_palaceRight].x = (dotJ < 12) ? viewports[v_palace].x + viewports[v_palace].w : viewports[v_palace].x - viewports[v_palace].w;
	viewports[v_palaceRight].y = viewports[v_palace].y;
	viewports[v_palaceRight].h = viewports[v_palace].w;
	viewports[v_palaceRight].w = viewports[v_palace].h;
}

void SetPalace(SDL_Rect* viewports, Player* player)
{
	viewports[v_palace].x = player->GetBox().x - (player->GetBox().w / 4 * 10);
	viewports[v_palace].y = player->GetBox().y - (player->GetBox().h / 4 * 10);
	viewports[v_palace].w = player->GetBox().w * 10;
	viewports[v_palace].h = player->GetBox().h * 10;
}

void SetFarLeftTile(SDL_Rect* viewports)
{
	viewports[v_farLeftTile].x = viewports[v_tile].x;
	viewports[v_farLeftTile].y = viewports[v_tile].y;
	viewports[v_farLeftTile].w = viewports[v_tile].w;
	viewports[v_farLeftTile].h = viewports[v_tile].h;
}

void SetFarTopTile(SDL_Rect* viewports)
{
	viewports[v_farTopTile].x = viewports[v_tile].x;
	viewports[v_farTopTile].y = viewports[v_tile].y;
	viewports[v_farTopTile].w = viewports[v_tile].w;
	viewports[v_farTopTile].h = viewports[v_tile].h;
}

void SetColumn(SDL_Rect* viewports)
{
	viewports[v_column] = viewports[v_tile];
}

void SetRow(SDL_Rect* viewports)
{
	viewports[v_row] = viewports[v_tile];
}

void SetPlayerMiracle(SDL_Rect* viewports, const int ROW_SIZE, float dotNormalJ)
{
	viewports[v_playerMiracle].x = (viewports[v_farLeftTile].x + ((viewports[v_farLeftTile].w * ROW_SIZE) * dotNormalJ) - (viewports[v_farLeftTile].w));
	viewports[v_playerMiracle].y = viewports[v_tile].y - (viewports[v_tile].h * 0.5f);
	viewports[v_playerMiracle].w = viewports[v_tile].w * 2.0f;
	viewports[v_playerMiracle].h = viewports[v_tile].h * 2.0f;
}

void SetPlayerClouds(SDL_Rect* viewports, const int ROW_SIZE, float dotNormalJ)
{
	viewports[v_playerClouds].x = (viewports[v_farLeftTile].x + ((viewports[v_farLeftTile].w * ROW_SIZE) * dotNormalJ) - (viewports[v_farLeftTile].w)) - (viewports[v_playerClouds].w * 0.1f);
	viewports[v_playerClouds].y = viewports[v_tile].y - (viewports[v_tile].h * 1.4f);
	viewports[v_playerClouds].w = viewports[v_tile].w * 2.6f;
	viewports[v_playerClouds].h = viewports[v_tile].h * 2.2f;
}

void SetArrow(SDL_Rect* viewports, double* normals)
{
	viewports[v_arrow].x = ((float)viewports[v_farTopTile].x < (float)viewports[v_player].x)
		? (float)viewports[v_farTopTile].x + (((float)viewports[v_player].x - (float)viewports[v_farTopTile].x) * (1.0f - normals[second_1]))
		: (float)viewports[v_player].x + (((float)viewports[v_farTopTile].x - (float)viewports[v_player].x) * normals[second_1]);
	viewports[v_arrow].y = (float)viewports[v_farTopTile].y + (((float)viewports[v_player].y - (float)viewports[v_farTopTile].y) * (1.0f - normals[second_1]));
	viewports[v_arrow].w = viewports[v_player].w - ((viewports[v_player].w - viewports[v_farTopTile].w) * normals[second_1]);
	viewports[v_arrow].h = viewports[v_player].h - ((viewports[v_player].h - viewports[v_farTopTile].h) * normals[second_1]);
}

void SetPlayerStarCurrent(SDL_Rect* viewports, SDL_Rect* miracleStarfallViewports, int index, double* miracleStarfallNormals)
{
	viewports[v_playerStarCurrent].x = miracleStarfallViewports[index].x;
	viewports[v_playerStarCurrent].y = miracleStarfallViewports[index].y + (window.GetHeight() * miracleStarfallNormals[index]);
	viewports[v_playerStarCurrent].w = miracleStarfallViewports[index].w;
	viewports[v_playerStarCurrent].h = miracleStarfallViewports[index].h;
}

void SetPlayerBox(SDL_Rect* viewports, const int ROW_SIZE, float dotNormalJ)
{
	viewports[v_playerBox].x = (viewports[v_farLeftTile].x + ((viewports[v_farLeftTile].w * ROW_SIZE) * dotNormalJ) - (viewports[v_farLeftTile].w));
	viewports[v_playerBox].y = viewports[v_tile].y - (viewports[v_tile].h * 0.5f) - (viewports[v_tile].h * 1.25f);
	viewports[v_playerBox].w = viewports[v_tile].w * 2.0f;
	viewports[v_playerBox].h = viewports[v_tile].h * 2.0f;
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

void SetMenuBottom(SDL_Rect* viewports)
{
	viewports[v].x = 0;
	viewports[v].y = window.GetHeight() - (window.GetHeight() * 0.7f);
	viewports[v].w = window.GetWidth();
	viewports[v].h = window.GetHeight();
}

void SetMenuRightTop(SDL_Rect* viewports)
{
	viewports[v].x = window.GetWidth() * 0.5f;
	viewports[v].y = window.GetHeight() - (window.GetHeight() * 0.95f);
	viewports[v].w = window.GetWidth() * 0.5f;
	viewports[v].h = window.GetHeight() * 0.6f;
}

void SetMenuRightBottom(SDL_Rect* viewports)
{
	viewports[v].x = window.GetWidth() * 0.5f;
	viewports[v].y = window.GetHeight() - (window.GetHeight() * 0.75f);
	viewports[v].w = window.GetWidth() * 0.5f;
	viewports[v].h = window.GetHeight() * 0.6f;
}

void SetMenuLeft(SDL_Rect* viewports)
{
	viewports[v].x = 0;
	viewports[v].y = window.GetHeight() - (window.GetHeight() * 0.9f);
	viewports[v].w = window.GetWidth() * 0.5f;
	viewports[v].h = window.GetHeight() * 0.6f;
}

void SetTileMiracle(SDL_Rect* viewports)
{
	viewports[v_tileMiracle].x = viewports[v_tile].x;
	viewports[v_tileMiracle].y = viewports[v_tile].y - viewports[v_tile].h;
	viewports[v_tileMiracle].w = viewports[v_tile].w;
	viewports[v_tileMiracle].h = viewports[v_tile].h;
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

void DrawBossMoonboy(SDL_Rect* viewports, int horizontalModifier)
{
	SetBoss(viewports, horizontalModifier);
	unsorted[CharacterTownspersonMoonboy].Draw(&viewports[v_boss]);
}

void DrawBossMoonboyHands(SDL_Rect* viewports, int horizontalModifier)
{
	SetBoss(viewports, horizontalModifier);
	unsorted[CharacterTownspersonMoonboy_Hands].Draw(&viewports[v_boss]);
}

void DrawBackgroundForeground(SDL_Rect* viewports)
{
	unsorted[BackgroundForeground].Draw(NULL);
}

void DrawAlphabet(SDL_Rect* viewports, int j)
{
	alphabet[ASCII065000 + j].Draw(&viewports[v_tileMiracle]);
}

void DrawHouseWithWind(SDL_Rect* viewports, int windIndex, double windAngle, bool isActive, int fireIndex)
{
	houseDenPillars[HouseDenPillar_000].Draw(&viewports[v_tile]);
	winds[windIndex].Draw(&viewports[v_tileMiracle], windAngle, SDL_FLIP_HORIZONTAL);
	(isActive)
		? redFire[fireIndex].Draw(&viewports[v_tileMiracle])
		: blueFire[fireIndex].Draw(&viewports[v_tileMiracle]);
}

void DrawPlayerHighlight(SDL_Rect* viewports)
{
	SetPlayer(viewports);
	unsorted[PlayerHighlight].Draw(&viewports[v_player]);
}

void DrawFarTopTile(SDL_Rect* viewports)
{
	SetFarTopTile(viewports);
	unsorted[PalaceHighlightBottom].Draw(&viewports[v_farTopTile]);
}

void DrawMenuTop(SDL_Rect* viewports)
{
	viewports[v].x = 0;
	viewports[v].y = -window.GetHeight() * 0.4f;
	viewports[v].w = window.GetWidth();
	viewports[v].h = window.GetHeight();
	unsorted[MenuTop].Draw(&viewports[v]);
}

int TextToIndex(char letter)
{
	return (int)letter - 33;
}

void DrawAltMenuTop(SDL_Rect* viewports, const char* text)
{
	const int COLUMN_LIMIT = 20;
	viewports[v].y = 0;
	viewports[v].w = window.GetWidth() / COLUMN_LIMIT;
	viewports[v].h = viewports[v].w;
	//TextToIndex(narrowText);
	for (int i = 0; text[i] != '\0'; ++i)
	{
		viewports[v].x = (window.GetWidth() / COLUMN_LIMIT) * i;
		alphabet[TextToIndex(text[i])].Draw(&viewports[v]);
	}
}

void DrawMenuBottom(SDL_Rect* viewports)
{
	SetMenuBottom(viewports);
	unsorted[MenuBottom].Draw(&viewports[v]);
}

void DrawMenuRightTop(SDL_Rect* viewports)
{
	SetMenuRightTop(viewports);
	unsorted[MenuRightTop].Draw(&viewports[v]);
}

void DrawMenuRightBottom(SDL_Rect* viewports)
{
	SetMenuRightBottom(viewports);
	unsorted[MenuRightBottom].Draw(&viewports[v]);
}

void DrawMenuLeft(SDL_Rect* viewports)
{
	SetMenuLeft(viewports);
	unsorted[MenuLeft].Draw(&viewports[v]);
}

void DrawCrest(SDL_Rect* viewports)
{
	unsorted[Crest].Draw(NULL);
}

void DrawEarthquake(SDL_Rect* viewports, SDL_Rect* miracleStarfallViewports, int index, double* miracleStarfallNormals)
{
	SetPlayerStarCurrent(viewports, miracleStarfallViewports, index, miracleStarfallNormals);
	characterFairyWistfuls[index].Draw(&viewports[v_playerStarCurrent]);
}

void DrawRightDownDown(SDL_Rect* viewports, int dotJ)
{
	SetRightDownDown(viewports, dotJ);
	unsorted[PalaceHighlightRight].Draw(&viewports[v_palaceRightDownDown]);
}

void DrawRightDown(SDL_Rect* viewports, int dotJ)
{
	SetRightDown(viewports, dotJ);
	unsorted[PalaceHighlightTop].Draw(&viewports[v_palaceRightDown]);
}

void DrawRight(SDL_Rect* viewports, int dotJ)
{
	SetRight(viewports, dotJ);
	unsorted[PlayerHighlight].Draw(&viewports[v_palaceRight]);
}

void DrawBoxFront(SDL_Rect* viewports, Player* player)
{
	SetPalace(viewports, player);
	unsorted[BoxFront].Draw(&viewports[v_palace]);
}

void DrawPalace(SDL_Rect* viewports, Player* player)
{
	SetPalace(viewports, player);
	unsorted[Loadstone].Draw(&viewports[v_palace]);
}

void DrawArrow(SDL_Rect* viewports, double* normals)
{
	SetArrow(viewports, normals);
	unsorted[IconCursor].Draw(&viewports[v_arrow], 0.0, SDL_FLIP_VERTICAL);
}

// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa
// https://learn.microsoft.com/en-us/windows/win32/procthread/process-creation-flags
// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/ns-processthreadsapi-startupinfoa
void WindowsProcess(char* narrowText)
{
#ifdef _WIN32
	STARTUPINFO startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&processInfo, sizeof(processInfo));
	size_t wideTextSize;                                                      // Declare a variable for wide text's size.
	mbstowcs_s(&wideTextSize, nullptr, 0, narrowText, _TRUNCATE);             // Define the wide text's size.
	wchar_t* wideText = new wchar_t[wideTextSize];                            // Declare wide text.
	mbstowcs_s(&wideTextSize, wideText, wideTextSize, narrowText, _TRUNCATE); // Define wide text.
	if (!CreateProcess(NULL, wideText, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startupInfo, &processInfo))
	{
		printf("ERROR: Failed to create process: %ul\n", GetLastError());
	}
	delete[] wideText;
	printf("SUCCESS: Created Process\n");
	printf("SUCCESS: Process ID, %i\n", processInfo.dwProcessId);
	printf("SUCCESS: Thread ID, %i\n", processInfo.dwThreadId);
	WaitForSingleObject(processInfo.hProcess, INFINITE);
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);
#endif
}

char** splitString(const std::string& inputString, int& tokenCount) {
    // Create a mutable copy of the input string as strtok modifies the string.
    char* cstr = new char[inputString.length() + 1];
    strcpy(cstr, inputString.c_str());

    // First pass to count tokens
    char* tempCstr = new char[inputString.length() + 1];
    strcpy(tempCstr, inputString.c_str());
    char* token = strtok(tempCstr, " ");
    tokenCount = 0;
    while (token != nullptr) {
        tokenCount++;
        token = strtok(nullptr, " ");
    }
    delete[] tempCstr; // Clean up temporary buffer

    // Allocate memory for the char** array
    char** tokensArray = new char*[tokenCount];

    // Second pass to store tokens
    int i = 0;
    token = strtok(cstr, " "); // Use the original mutable copy
    while (token != nullptr) {
        tokensArray[i] = new char[strlen(token) + 1]; // Allocate for each token
        strcpy(tokensArray[i], token);
        token = strtok(nullptr, " ");
        i++;
    }

    delete[] cstr; // Clean up the mutable copy of the input string
    return tokensArray;
}

void LinuxProcess(char* text)
{
#ifdef __linux__

	/*
	int spaceCount = 0;
	int textLimit = strlen(text);
	for (int i = 0; i < textLimit; ++i)
	{
		if (text[i] == ' ')
		{
			++spaceCount;
		}
	}
	++spaceCount;
	printf("space count: %i\n", spaceCount);
	char modifiedText[textLimit];
	//modifiedText[textLimit] = nullptr;
	for (int i = 0; i < textLimit; ++i)
	{
		if (text[i] == ' ')
		{
			modifiedText[i] = '\0';
		}
		else
		{
			modifiedText[i] = text[i];
		}
	}
	printf("text: %s\n", text);
	printf("mod : %s\n", modifiedText);
	char* texts[spaceCount + 1];
	texts[spaceCount] = NULL;
	texts[0] = modifiedText;
	if (spaceCount > 1)
	{
		int j = 1;
		for (int i = 0; i < textLimit; ++i)
		{
			if (modifiedText[i] == '\0')
			{
				printf(">> %s\n", &modifiedText[i + 1]);
				texts[j] = &modifiedText[i + 1];
				++j;
			}
		}
	}
	for (int i = 0; i < spaceCount + 1; ++i)
	{
		printf("texts[%i]: %s\n", i, texts[i]);
	}
	*/

	//std::string sentence = "This is a sample string to split";
    int count = 0;
    char** words = splitString(text, count);


    std::cout << "Tokens:" << std::endl;
    for (int i = 0; i < count; ++i) {
        std::cout << words[i] << std::endl;
    }

	/*char* token;
	token = strtok(text, " ");
	int tokensLimit = strlen(token);//WRONG
	if (tokensLimit == strlen(text))
	printf("\nTOKEN LENGTH: %i\n", tokensLimit);
	char* tokens[tokensLimit + 1];
	int tokenCounter = 0;
	while (token != NULL) {
		tokens[tokenCounter] = token;
        token = strtok(NULL, " \t\n");
		++tokenCounter;
    }
    tokens[tokenCounter] = nullptr;
	for (int i = 0; i < tokensLimit; ++i)
	{
		printf("tokens[%i]: %s\n", i, tokens[i]);
	}*/
	//execl("/bin/ls", text, NULL);
	pid_t pid = fork();
    if (pid == -1) perror("fork failed");
    else if (pid == 0) // Child.
	{
        std::cout << "Child process (PID: " << getpid() << ") is about to execute a new program." << std::endl;
        execv("/bin/ls", words);
        //perror("execv failed");
    }
    else // Parent.
	{
        std::cout << "Parent process (PID: " << getpid() << ") is waiting for child (PID: " << pid << ")." << std::endl;
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to complete
        std::cout << "Parent process: Child finished with status " << WEXITSTATUS(status) << std::endl;
    }
#endif
}
