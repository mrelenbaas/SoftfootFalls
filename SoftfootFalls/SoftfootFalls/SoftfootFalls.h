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


#ifdef _WIN32
const char SEPARATOR = '\\';
#elif __linux__
const char SEPARATOR = '/';
#endif

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

enum PathEnum
{
	Crest,
	BackgroundBackground,
	BackgroundForeground,
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
	CharacterFairyHopeful,
	PathEnum_Size
};
const char* Paths[] =
{
	"Crest_2048x2048_000.png",
	"bg.png",
	"Landscape_Moon_3300x2550.png",
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
	"PlayerBeam_000_2048x2048.png",
	"CharacterFairyHopeful_000_256x256.png"
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
