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
		auto _Print = [](int size, int totalSize, int offset, const char* text, int number)
		{
			auto PrintWithClassName = [](const char* text, long long number)
			{
				printf("%s: %lld\n", text, number);
			};

			char* name = new char[size];
			memset(name, 0, size);
			for (int i = 0, j = offset; i < size - 1 && j < totalSize; ++i, ++j)
				name[i] = text[j];
			PrintWithClassName(name, number);
			delete[] name;
		};

		int offset = 0;
		int size = 0;
		if (isdigit(text[0]))
		{
			offset = 1;
			size = text[0] - '0' + 1;
			
			char* name = new char[size];
			memset(name, 0, size);
			for (int i = 0, j = offset; i < size - 1 && j < size; ++i, ++j)
				name[i] = text[j];
			_Print(size, size, offset, text, number);
			delete[] name;
		}
		else //PrintWithClassName(text, number);
		{
			offset = 0;
			int totalSize = 0;
			for (char* c = (char*)text; *c != '\0'; ++c)
			{
				++totalSize;
				if (isspace(*c))
				{
					offset = totalSize;
				}
			}
			size = totalSize - offset + 1;
			
			char* name = new char[size];
			memset(name, 0, size);
			for (int i = 0, j = offset; i < size - 1 && j < totalSize; ++i, ++j)
				name[i] = text[j];
			_Print(size, totalSize, offset, text, number);
			delete[] name;
		}
	};
	
	PrintTrimmed(typeid(*this).name(), Now() - start);
}

long long Clock::Now()
{
	return std::chrono::system_clock::now().time_since_epoch().count();
}
