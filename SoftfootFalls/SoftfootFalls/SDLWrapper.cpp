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
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MOUSE_ENTER:
#elif __linux__
	case SDL_WINDOWEVENT_ENTER:
#endif
		mouseFocus = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MOUSE_LEAVE:
#elif __linux__
	case SDL_WINDOWEVENT_LEAVE:
#endif
		mouseFocus = false;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_FOCUS_GAINED:
#elif __linux__
	case SDL_WINDOWEVENT_FOCUS_GAINED:
#endif
		keyboardFocus = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_FOCUS_LOST:
#elif __linux__
	case SDL_WINDOWEVENT_FOCUS_LOST:
#endif
		keyboardFocus = false;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MINIMIZED:
#elif __linux__
	case SDL_WINDOWEVENT_MINIMIZED:
#endif
		minimized = true;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_MAXIMIZED:
#elif __linux__
	case SDL_WINDOWEVENT_MAXIMIZED:
#endif
		minimized = false;
		break;
#ifdef _WIN32
	case SDL_EVENT_WINDOW_RESTORED:
#elif __linux__
	case SDL_WINDOWEVENT_RESTORED:
#endif
		minimized = false;
		break;
	case KEY_PRESSED:
		if (Key(event) == SDLK_RETURN)
		{
			if (fullscreen)
			{
#ifdef _WIN32
				SDL_SetWindowFullscreen(window, false);
#elif __linux__
				SDL_SetWindowFullscreen(window, 0);
#endif
				fullscreen = false;
			}
			else
			{
#ifdef _WIN32
				SDL_SetWindowFullscreen(window, true);
#elif __linux__
				SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif
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
	mouseFocus = false;
	keyboardFocus = false;
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

bool LWindow::HasMouseFocus() const
{
	return mouseFocus;
}

bool LWindow::HasKeyboardFocus() const
{
	return keyboardFocus;
}

bool LWindow::IsMinimized() const
{
	return minimized;
}
