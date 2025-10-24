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


class Window
{
public:
	Window()
		: window(nullptr)
		, renderer(nullptr)
		, title("SoftfootFalls\0")
		, width(640)
		, height(480)
		, fullscreen(false)
		, minimized(false) {
	}
	bool Init();
	void HandleEvent(SDL_Event&);
	void Free();
	int GetWidth() const { return width; };
	int GetHeight() const { return height; };
	SDL_Renderer* GetRenderer() { return renderer; };
	SDL_Window* GetWindow() { return window; };
	void SetRenderer(SDL_Renderer*);
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
