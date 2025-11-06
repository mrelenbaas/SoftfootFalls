#include <stdio.h>

#ifdef _WIN32
#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#elif __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif