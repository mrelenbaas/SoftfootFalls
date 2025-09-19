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

/**
 * @brief Print the FPS averages over the last 10 seconds.
 */
void Timer::Print()
{
	Timer::framesPerSeconds[Timer::index] = Timer::framesPerSecond.load();
	Timer::framesPerSecond.store(0);
	int average = 0;
	for (int i = 0; i < Timer::size; ++i)
		average += Timer::framesPerSeconds[i].load();
	average /= Timer::size;
	++Timer::index;
	if (Timer::index.load() >= Timer::size) Timer::index.store(0);
	Print::WithClassName(typeid(Timer).name(), average);
}

/**
 * @brief Calls a callback after sleeping on a detatched thread.
 * 
 * @param The callback function.
 * @param wait The time to sleep between callback calls.
 */
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

/**
 * @brief Empty.
 */
Timer::~Timer()
{
}

/**
 * @brief Add one frame to the FPS counter.
 */
void Timer::Update()
{
	++Timer::framesPerSecond;
}
