#include <iostream>

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
	void Move(int, int, long long, double);
	SDL_Rect GetBox()
	{
		SDL_Rect rect =
		{
			box.x,
			box.y,
			box.w,
			box.h
		};
		return rect;
	}
	void SetIJ(int*, int*, float*, float*, int, int, const int) const;
	void SetUp(bool isUp)
	{
		(*this).isUp = isUp;
	}
	void SetDown(bool isDown)
	{
		(*this).isDown = isDown;
	}
	void SetLeft(bool isLeft)
	{
		(*this).isLeft = isLeft;
	}
	void SetRight(bool isRight)
	{
		(*this).isRight = isRight;
	}
private:
	SDL_Rect box { 0, 0, DOT_WIDTH, DOT_HEIGHT };
	long long previousTime = 0L;
	bool isUp = false;
	bool isDown = false;
	bool isLeft = false;
	bool isRight = false;
};
