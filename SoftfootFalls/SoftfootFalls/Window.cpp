#include "Window.h"
#include "SDLInterface.h"


bool Window::Init()
{
	SecondInit(title, width, height, &window, &renderer);
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
	case KEY_PRESSED:
		if (Key(event) == SDLK_END)
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
			}
		}
		break;
	}
}

void Window::Free()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	width = 0;
	height = 0;
}

void Window::SetRenderer(SDL_Renderer* renderer)
{
	(*this).renderer = renderer;
}
