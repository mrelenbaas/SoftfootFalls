#include "SDLWrapper.h"
#include "SDLInterface.h"


bool IsResized(SDL_Event& event)
{
	bool result = false;
#ifdef _WIN32
	if (event.type == SDL_EVENT_WINDOW_RESIZED)
#elif __linux__
	if ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED))
#endif
	{
		result = true;
	}
	return result;
}

void LWindow::HandleEvent(SDL_Renderer* renderer, SDL_Event& event)
{
	if (IsResized(event))
	{
		width = event.window.data1;
		height = event.window.data2;
		printf("TODO, WINDOWS: THIS IS IN 2 DIFFERENT FILES width: %i, height: %i\n", width, height);
	}
	switch (event.type)
	{
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
