#include <stdio.h>
#include <cctype>
#include <cstring>

#include "Print.h"


void Print::WithClassName(const char* text, long long number)
{
	auto _Print = [](int size, int offset, const char* text, long long number)
	{
		auto PrintStringAndInt = [](const char* text, long long number)
		{
			printf("%s: %lld\n", text, number);
		};
		char* name = new char[size];
		memset(name, 0, size);
		for (int i = 0, j = offset; i < size; ++i, ++j) name[i] = text[j];
		PrintStringAndInt(name, number);
		delete[] name;
	};
	int offset = 0;
	int size = 0;
	if (isdigit(text[0]))
	{
		offset = 1;
		size = text[0] - '0' + 1;
	}
	else
	{
		offset = 0;
		int total = 0;
		for (char* c = (char*)text; *c != '\0'; ++c)
		{
			++total;
			if (isspace(*c)) offset = total;
		}
		size = total - offset + 1;
	}
	_Print(size, offset, text, number);
}