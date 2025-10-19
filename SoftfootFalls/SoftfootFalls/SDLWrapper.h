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
		, width(0)
		, height(0) {}
	bool Init();
#ifdef __linux__
	SDL_Renderer* CreateRenderer(); // TODO: Update this while in Linux.
#endif
	SDL_Renderer* GetRenderer();
	void HandleEvent(SDL_Event&);
	void Free();
	int GetWidth() const;
	int GetHeight() const;
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
