#include <stdio.h>

#ifdef _WIN32
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif


class FileIO
{
public:
	static const int TOTAL_DATA = 10;
	static void Open(const char*, Sint32*);
	static void Write(const char*, Sint32*);
};