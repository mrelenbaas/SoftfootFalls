#include "Clock.h"


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
	auto PrintTrimmed = [](const char* text, long long number)
	{
		auto PrintWithClassName = [](const char* text, long long number)
		{
			printf("%s: %lld\n", text, number);
		};
	
		if (isdigit(text[0]))
		{
			int size = text[0] - '0' + 1;
			char* name = new char[size];
			memset(name, 0, size);
			for (int i = 0, j = 1; j < size; ++i, ++j)
				name[i] = text[j];
			PrintWithClassName(name, number);
			delete[] name;
		}
		else __Print(text, number);
	};
	
	PrintTrimmed(typeid(*this).name(), Now() - start);
}

long long Clock::Now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}
