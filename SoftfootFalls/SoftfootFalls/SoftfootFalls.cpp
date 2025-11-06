#include "SoftfootFalls.h"

#include "Time.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib>

#include "Point.h"


int main(int argc, char* argv[])
{
	if (argc < 1) return 1;
	bool isAcceptingInput = true;
	int alphabetIndex = 0;

	const char* BASE_PATH = BasePath(argv[0]);
	Load* load = new Load(BASE_PATH);
	if (!window.Init()) return 1;
	SDL_Renderer* linuxRenderer = GetLinuxRenderer(window.GetWindow());
	if (linuxRenderer) window.SetRenderer(linuxRenderer);
	LoadArt(load);

	const char* temp = "Prompt\0";
	char* text = new char[ROW_SIZE];
	int copyI = 0;
	int copyJ = 0;
	for (copyI = 0; temp[copyI] != '\0'; ++copyI)
	{
		text[copyI] = temp[copyI];
	}
	text[copyI] = '\0';
	char* texts[ROW_SIZE]{};
	for (int copyI = 0; copyI < ROW_SIZE; ++copyI)
	{
		texts[copyI] = new char[ROW_SIZE];
		for (copyJ = 0; copyJ < ROW_SIZE - 1; ++copyJ)
		{
			texts[copyI][copyJ] = '\0';
		}
		texts[copyI][ROW_SIZE - 1] = '\0';
	}
	while (window.IsRunning())
	{
		long long currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		for (int i = 0; i < TimerEnum_Size; ++i) deltas[i] += currentTime - previousTime;
		if (deltas[animation] > limits[animation])
		{
			deltas[animation] -= limits[animation];
			if (animationIndex++ >= ANIMATION_LIMIT) animationIndex = 0;
		}
		if (deltas[minute_1] > limits[minute_1]) minuteToggle = !minuteToggle;
		for (int i = 0; i < TimerEnum_Size; ++i)
		{
			if (deltas[i] > limits[i]) deltas[i] -= limits[i];
			normals[i] = (double)deltas[i] / limits[i];
		}
		previousTime = currentTime;
		SDL_Event event;
		while (SDL_PollEvent(&event) != 0)
		{
			if (IsWindowQuit(event)) window.Quit();
			else if (event.type == KEY_RELEASED)
			{
				switch (Key(event))
				{
				case SDLK_HOME:
					break;
				case KEY_W:
					player.SetUp(false);
					break;
				case KEY_S:
					player.SetDown(false);
					break;
				case KEY_A:
					player.SetLeft(false);
					break;
				case KEY_D:
					player.SetRight(false);
					break;
				}
			}
			else if (event.type == KEY_PRESSED)
			{
				if (isAcceptingInput)
				{
					//printf("event: %i\n", (int)Key(event));
#ifdef _WIN32
					SDL_Keycode key_code = event.key.key;
					if (key_code >= SDLK_SPACE && key_code <= SDLK_Z)
#elif __linux__
					SDL_Keycode key_code = event.key.keysym.sym;
					if (key_code >= SDLK_SPACE && key_code <= SDLK_z)
#endif
					{
						char ascii_char = static_cast<char>(key_code);
						printf("%c", ascii_char);
						// Now you have the ASCII character
						copyI = 0;
						while (copyI < ROW_SIZE - 2)
						{
							if (text[copyI] == '\0')
							{
								text[copyI + 1] = '\0';
								text[copyI] = ascii_char;
								break;
							}
							++copyI;
						}
					}
					else
					{
						switch (Key(event))
						{
						case SDLK_BACKSPACE:
							copyI = 0;
							while (copyI < ROW_SIZE - 1)
							{
								if (text[copyI] == '\0')
								{
									text[copyI - 1] = '\0';
									break;
								}
								++copyI;
							}
							break;
						case SDLK_RETURN:
							delete[] texts[ROW_SIZE - 1];
							for (int i = ROW_SIZE - 2; i >= 0; --i)
							{
								texts[i + 1] = texts[i];
								for (copyI = 0; texts[i + 1][copyI] != '\0'; ++copyI)
								{
									//texts[0][copyI] = text[copyI];
									texts[i + 1][copyI] = texts[i][copyI];
								}
							}
							texts[0] = new char[ROW_SIZE];
							//copyI = 0;
							for (copyI = 0; text[copyI] != '\0'; ++copyI)
							{
								texts[0][copyI] = text[copyI];
								text[copyI] = '\0';
							}
							texts[0][copyI] = '\0';
							//printf("%s\n", texts[0]);
							//for (int i = 0; i < ROW_SIZE; ++i)
							//{
							//	printf("%s\n", texts[i]);
							//}
#ifdef _WIN32
							WindowsProcess(texts[0]);
#elif __linux__
							LinuxProcess(texts[0]);
#endif
							break;
						case SDLK_INSERT:
							isAcceptingInput = !isAcceptingInput;
							break;
						}
					}
					continue;
				}
				switch (Key(event))
				{
				case SDLK_HOME:
					break;
				case SDLK_INSERT:
					isAcceptingInput = !isAcceptingInput;
					break;
				case KEY_W:
					player.SetUp(true);
					break;
				case KEY_S:
					player.SetDown(true);
					break;
				case KEY_A:
					player.SetLeft(true);
					break;
				case KEY_D:
					player.SetRight(true);
					break;
				}
			}
			window.HandleEvent(event);
		}
		if (window.IsMinimized()) continue;
		SDL_RenderClear(window.GetRenderer());
		DrawBackground(viewports);
		DrawForeground(viewports);

		player.Move(window.GetWidth(), window.GetHeight(), currentTime, limits[second_3]);
		player.SetIJ((float)window.GetWidth(), (float)window.GetHeight(), ROW_SIZE);
		float normal = normals[second_1];
		float centerX = window.GetWidth() * 0.15f;
		float centerY = window.GetHeight() * 0.5f;
		int roadI = 0;
		int k = 0;
		for (int i = ROW_SIZE - 1; i >= 0 ; --i)
		{
			normal = (float)i / (float)ROW_SIZE;
			viewports[v_tile].w = centerX / ROW_SIZE;
			viewports[v_tile].h = centerY / ROW_SIZE;
			viewports[v_tile].x = ((0 + centerX) * normal);
			viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
			SetRow(viewports);
			viewports[v_tile].w = centerX / ROW_SIZE;
			viewports[v_tile].h = centerY / ROW_SIZE;
			viewports[v_tile].x = (window.GetWidth() - viewports[v_tile].w - ((0 + centerX) * normal));
			viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
			SetColumn(viewports);
			for (int j = 0; j < ROW_SIZE; ++j)
			{
				float xStep = (float)(viewports[v_column].x + viewports[v_column].w - viewports[v_row].x) / ROW_SIZE;
				viewports[v_tile].w = (viewports[v_column].x + viewports[v_column].w - viewports[v_row].x) / ROW_SIZE;
				viewports[v_tile].h = viewports[v_tile].w;
				viewports[v_tile].x = (viewports[v_row].x + (j * xStep));
				viewports[v_tile].y = (window.GetHeight() - viewports[v_tile].h - ((0 + centerY) * normal));
				if (viewports[v_tile].x < 0) viewports[v_tile].x = 0;
				if (viewports[v_tile].y < 0) viewports[v_tile].y = 0;
				if (viewports[v_tile].w < 0) viewports[v_tile].w = 0;
				if (viewports[v_tile].h < 0) viewports[v_tile].h = 0;
				int modI = (i % 2 == 0) ? 0 : 4;
				modI = (i % 4 == 0);
				roads[modI + roadI].Draw(&viewports[v_tile]);
				++roadI;
				if (roadI > 3) roadI = 0;
				if (player.GetI() == i && player.GetJ() == j) DrawPlayerHighlight(viewports);
				if (i == ROW_SIZE - 1 && player.GetJ() == j) DrawFarTopTile(viewports);
				if (j == 0) SetFarLeftTile(viewports);
				if (i == player.GetI()) unsorted[PalaceHighlightLeft].Draw(&viewports[v_farLeftTile]);
				SetPlayerBox(viewports, ROW_SIZE, player.GetJNormal());
				SetPlayerMiracle(viewports, ROW_SIZE, player.GetJNormal());
				SetPlayerClouds(viewports, ROW_SIZE, player.GetJNormal());
				if (j == alphabetIndex)
				{
					if (texts[i][alphabetIndex] == '\0')
					{
						alphabetIndex = 0;
					}
					else
					{
						alphabet[TextToIndex(texts[i][alphabetIndex])].Draw(&viewports[v_tile]);
						++alphabetIndex;
					}
				}
				SetTileMiracle(viewports);
				++k;
			}
		}
		DrawArrow(viewports, normals);
		SetPalace(viewports, &player);
		if (isAcceptingInput) DrawShellPrompt(viewports, text);
		else DrawMenuTop(viewports);
		SDL_RenderPresent(window.GetRenderer());
	}
	for (int i = 0; i < ROW_SIZE; ++i)
	{
		delete[] texts[i];
	}
	delete[] text;
	UnloadArt();
	window.Free();
	SDL_Quit();
	delete load;
	delete[] BASE_PATH;
	return 0;
}
