#include "SDLWrapper.h"
#include "SDLInterface.h"


void FileIO::Open(const char* path, Sint32* data)
{
	auto file = GetFile();
	file = IOFromFile(path, "r+b");
	if (file == NULL)
	{
		SDL_Log("Warning: Unable to open file! SDL Error: %s\n", SDL_GetError());
		file = IOFromFile(path, "w+b");
		if (file != NULL)
		{
			SDL_Log("New file created!\n");
			for (int i = 0; i < TOTAL_DATA; ++i)
			{
				data[i] = 0;
				WriteIO(file, &data[i]);
			}
			CloseIO(file);
		}
		else
		{
			SDL_Log("Error: Unable to create file! SDL Error: %s\n", SDL_GetError());
			//return 1;
		}
	}
	else
	{
		SDL_Log("Reading file...!\n");
		for (int i = 0; i < TOTAL_DATA; ++i) ReadIO(file, &data[i]);
		CloseIO(file);
	}
}

void FileIO::Write(const char * path, Sint32* data)
{
	auto file = GetFile();
	file = IOFromFile(path, "w+b");
	if (file != NULL)
	{
		for (int i = 0; i < TOTAL_DATA; ++i) WriteIO(file, data);
		CloseIO(file);
		printf("SUCCESS: Saved file: %s\n", path);
	}
	else printf("ERROR: Failed to save file: %s\n", SDL_GetError());
}
