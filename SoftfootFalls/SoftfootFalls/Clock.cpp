#include <stdio.h>
#include <chrono>
#include <typeinfo>

#include "Clock.h"
#include "Time.h"
#include "Print.h"


/**
 * @brief Starts the clock.
 */
Clock::Clock()
{
	start = Time::Now();
}

/**
 * @brief Prints the duration since the constructor was called.
 */
Clock::~Clock()
{
	Print();
}

void Clock::Print() const
{
	Print::WithClassName(typeid(*this).name(), Time::Now() - start);
}
