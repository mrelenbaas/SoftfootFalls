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
	LWindow();
	bool Init(bool bCreateRenderer = true);
#ifdef __linux__
	SDL_Renderer* createRenderer(); // TODO: Update this while in Linux.
#endif
	SDL_Renderer* GetRenderer();
	void HandleEvent(SDL_Event&);
	void Free();
	int GetWidth();
	int GetHeight();
	SDL_Window* GetWindow();
	bool HasMouseFocus();
	bool HasKeyboardFocus();
	bool IsMinimized();
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width;
	int height;
	bool mouseFocus;
	bool keyboardFocus;
	bool fullScreen;
	bool minimized;
};
