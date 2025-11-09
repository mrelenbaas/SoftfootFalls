#include "Window.h"
#include "SDLInterface.h"
#include <cassert>


void Window::Init()
{
	SecondInit(title, width, height, &window, &renderer);
	assert(&window != NULL && "ERROR: Failed to create Window.\n");
	printf("SUCCESS: Window created.\n");
	assert(&renderer != NULL && "ERROR: Failed to create Renderer.\n");
	printf("SUCCESS: Created Renderer.\n");
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
