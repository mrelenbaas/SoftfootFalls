#include <stdio.h>
#include <iostream>

#include "Print.h"
#include "Clock.h"

#include "SoftfootFalls.h"


int main()
{
    Clock* clock = new Clock();

    printf("Hello World\n");

    delete clock;
    return 0;
}
