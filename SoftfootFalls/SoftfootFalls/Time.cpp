#include <chrono>

#include "Time.h"


long long Time::Now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}