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


class Player
{
public:
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;
	static const int DOT_VEL = 100;
	Player() {}
	~Player() {}
	void Move(int, int, long long, double);
	int GetI() { return i; }
	int GetJ() { return j; }
	float GetINormal() { return iNormal; }
	float GetJNormal() { return jNormal; }
	SDL_Rect GetBox()
	{
		SDL_Rect rect = { box.x, box.y, box.w, box.h };
		return rect;
	}
	void SetIJ(int, int, const int);
	void SetUp(bool isUp) { (*this).isUp = isUp; }
	void SetDown(bool isDown) { (*this).isDown = isDown; }
	void SetLeft(bool isLeft) { (*this).isLeft = isLeft; }
	void SetRight(bool isRight) { (*this).isRight = isRight; }
private:
	SDL_Rect box{ 0, 0, DOT_WIDTH, DOT_HEIGHT };
	long long previousTime = 0L;
	bool isUp = false;
	bool isDown = false;
	bool isLeft = false;
	bool isRight = false;
	int i = 0;
	int j = 0;
	int iNormal = 0.0f;
	int jNormal = 0.0f;
};
