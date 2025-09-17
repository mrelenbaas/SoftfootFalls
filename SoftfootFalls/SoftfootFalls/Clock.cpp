#include <stdio.h>
#include <chrono>
#include <typeinfo>

#include "Clock.h"
#include "Print.h"


/**
 * @brief Starts the clock.
 */
Clock::Clock()
{
	start = Now();
}

/**
 * @brief Prints the duration since the constructor was called.
 */
Clock::~Clock()
{
	Print();
}

void Clock::Print()
{
	Print::WithClassName(typeid(*this).name(), Now() - start);
}

long long Clock::Now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}
