// SoftfootFalls.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>                                                       
#include <iostream>
#include <chrono>


int main()
{
    using namespace std::chrono;

    system_clock::time_point start = system_clock::now();

    printf("Hello World\n");

    system_clock::time_point stop = system_clock::now();
	long long duration = duration_cast<nanoseconds>(stop - start).count();

    printf("Duration: %lld", duration);
}