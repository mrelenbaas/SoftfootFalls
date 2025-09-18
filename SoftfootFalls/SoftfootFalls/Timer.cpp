#include <stdio.h>
#include <thread>
#include <functional>
#include <atomic>

#include "Timer.h"
#include "Print.h"
#include "Time.h"


std::atomic<bool> Timer::isReady{ false };
std::atomic<int> Timer::index{ 0 };
std::atomic<int> Timer::framesPerSecond{ 0 };
std::atomic<int> Timer::framesPerSeconds[Timer::size] = {};

void Timer::Print()
{
	Timer::framesPerSeconds[Timer::index] = Timer::framesPerSecond.load();
	Timer::framesPerSecond.store(0);
	//if (Timer::index.load() < Timer::size && !Timer::isReady.load())
	//{
	//	return;
	//}
	//else if (!Timer::isReady.load())
	//{
	//	Timer::isReady.store(true);
	//}
	int average = 0;
	for (int i = 0; i < Timer::size; ++i)
	{
		if (i == index)
		{
			printf("%i <\n", Timer::framesPerSeconds[i].load());
		}
		else
		{
			printf("%i\n", Timer::framesPerSeconds[i].load());
		}
		average += Timer::framesPerSeconds[i].load();
	}
	average /= Timer::size;
	++Timer::index;
	if (Timer::index.load() >= Timer::size)
	{
		Timer::index.store(0);
	}
	Print::WithClassName(typeid(Timer).name(), average);
}

Timer::Timer(std::function<void()> callback, int wait)
{
	using namespace std;

	std::thread([callback, wait]()
	{
		while (true)
		{
			callback();
			this_thread::sleep_for(chrono::seconds(wait));
		}
	}).detach();
}

Timer::~Timer()
{
}

void Timer::Update()
{
	++Timer::framesPerSecond;
}