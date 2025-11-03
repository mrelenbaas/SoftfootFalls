#include "Player.h"

void Dot::Move(int width, int height, long long currentTime, double limit)
{
	double normal = (currentTime - previousTime) / limit;
	double potentialX = 0.0;
	if (isLeft)
	{
		potentialX = box.x - (width * normal);
	}
	if (isRight)
	{
		potentialX = box.x + (width * normal);
	}
	if (potentialX > 0 && potentialX < width - (box.w * 4.0))
	{
		box.x = potentialX;
	}
	double potentialY = 0.0;
	if (isUp)
	{
		potentialY = box.y - (height * normal);
	}
	if (isDown)
	{
		potentialY = box.y + (height * normal);
	}
	if (potentialY > 0 && potentialY < height - (box.h * 4.0))
	{
		box.y = potentialY;
	}
	previousTime = currentTime;
}

void Dot::SetIJ(int* i, int* j, float* normalI, float* normalJ, int width, int height, const int LIMIT) const
{
	float w = width - (box.w * 4.0f);
	float h = height - (box.h * 4.0f);
	float x = (float)box.x;
	float y = (float)box.y;
	float jNormal = x / w;
	float iNormal = y / h;
	*i = LIMIT - (LIMIT * iNormal);
	if (*i == LIMIT) *i = LIMIT - 1;
	*j = LIMIT * jNormal;
	if (*j == LIMIT) *j = LIMIT - 1;
	*normalI = iNormal;
	*normalJ = jNormal;
}
