#include "Window.h"
#include "SDLInterface.h"


bool Window::Init()
{
	MainInit(title, width, height, &window, &renderer);
	return window != NULL;
}

void Window::HandleEvent(SDL_Event& event)
{
	if (IsResized(event))
	{
		width = event.window.data1;
		height = event.window.data2;
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
		break;
	}
}

void Window::Free()
{
	SDL_DestroyWindow(window);
	//SDL_DestroyRenderer(renderer);
	width = 0;
	height = 0;
}

int Window::GetWidth() const
{
	return width;
}

int Window::GetHeight() const
{
	return height;
}

SDL_Renderer* Window::GetRenderer()
{
	return renderer;
}

void Window::SetRenderer(SDL_Renderer* renderer)
{
	(*this).renderer = renderer;
}

SDL_Window* Window::GetWindow()
{
	return window;
}

bool Window::IsMinimized() const
{
	return minimized;
}
