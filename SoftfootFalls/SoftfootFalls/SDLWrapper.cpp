#include "SDLWrapper.h"
#include "SDLInterface.h"


void LWindow::HandleEvent(SDL_Renderer* renderer, SDL_Event& event)
{
	switch (event.type)
	{
#ifdef _WIN32
	case SDL_EVENT_WINDOW_RESIZED:
#endif
		width = event.window.data1;
		height = event.window.data2;
		printf("TODO, WINDOWS: THIS IS IN 2 DIFFERENT FILES width: %i, height: %i\n", width, height);
		break;
	case EVENT_WINDOW_MINIMIZED:
		minimized = true;
		break;
	case EVENT_WINDOW_MAXIMIZED:
		minimized = false;
		break;
	case EVENT_WINDOW_RESTORED:
		minimized = false;
		break;
	case KEY_PRESSED:
		if (Key(event) == SDLK_RETURN)
		{
			if (fullscreen)
			{
				SDL_SetWindowFullscreen(window, false);
				fullscreen = false;
			}
			else
			{
				SDL_SetWindowFullscreen(window, true);
				fullscreen = true;
				minimized = false;
			}
		}
		break;
	}
}

void LWindow::Free()
{
	if (window != NULL) SDL_DestroyWindow(window);
	width = 0;
	height = 0;
}

int LWindow::GetWidth() const
{
	return width;
}

int LWindow::GetHeight() const
{
	return height;
}

void LWindow::SetWidth(int width)
{
	(*this).width = width;
}

void LWindow::SetHeight(int height)
{
	(*this).height = height;
}

SDL_Window* LWindow::GetWindow()
{
	return window;
}

bool LWindow::IsMinimized() const
{
	return minimized;
}
