#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

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


class LWindow
{
public:
	LWindow()
		: window(nullptr)
		, renderer(nullptr)
		, title("SoftfootFalls\0")
		, width(640)
		, height(480)
		, fullscreen(false)
		, minimized(false) {}
	bool LInit();
	void HandleEvent(SDL_Renderer*, SDL_Event&);
	void Free();
	int GetWidth() const;
	int GetHeight() const;
	SDL_Renderer* GetRenderer();
	void SetWidth(int);
	void SetHeight(int);
	SDL_Window* GetWindow();
	bool IsMinimized() const;
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	const char* title;
	int width;
	int height;
	bool fullscreen;
	bool minimized;
};
