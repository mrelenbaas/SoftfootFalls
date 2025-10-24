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

struct Index
{
	int i;
	int j;
};

struct Point
{
	float x;
	float y;
};

class Container
{
protected:
	Container();
	virtual ~Container();
	Index index;
};
