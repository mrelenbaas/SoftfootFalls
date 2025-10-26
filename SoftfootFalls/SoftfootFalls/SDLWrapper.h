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


class Dot
{
public:
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	static const int DOT_VEL = 100;
	Dot() {}
	~Dot() {}
	void HandleEvent(SDL_Event&);
	void move(double timestepX, double timestepY);
	SDL_Rect GetBox()
	{
		SDL_Rect rect =
		{
			mBox.x,
			mBox.y,
			mBox.w,
			mBox.h
		};
		return rect;
	}
	void setIJ(int*, int*, float*, float*) const;
private:
	SDL_Rect mBox { 0, 0, DOT_WIDTH, DOT_HEIGHT };
	int mVelX = 0;
	int mVelY = 0;
};
