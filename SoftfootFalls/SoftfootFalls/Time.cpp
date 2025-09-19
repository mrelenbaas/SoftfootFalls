#include <chrono>

#include "Time.h"


/**
 * @brief Time since epoch in nanoseconds.
 */
long long Time::Now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}