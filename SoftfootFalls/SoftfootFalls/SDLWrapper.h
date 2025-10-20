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
		, mouseFocus(false)
		, keyboardFocus(false)
		, fullscreen(false)
		, minimized(false)
		, width(640)
		, height(480) {}
	bool Init();
#ifdef __linux__
	SDL_Renderer* CreateRenderer();
#endif
	SDL_Renderer* GetRenderer();
	void HandleEvent(SDL_Renderer*, SDL_Event&);
	void Free();
	int GetWidth() const;
	int GetHeight() const;
	void SetWidth(int);
	void SetHeight(int);
	SDL_Window* GetWindow();
	bool HasMouseFocus() const;
	bool HasKeyboardFocus() const;
	bool IsMinimized() const;
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	bool mouseFocus;
	bool keyboardFocus;
	bool fullscreen;
	bool minimized;
	int width;
	int height;
};
