#include "timer.h"
#include <stdio.h>

void Timer::update()
{
	IComponent::update();
	if (getCounter() == maxCounter)
	{
		kill(); // timer kills itself
		pushMsg(msg);
	}
}

Timer::~Timer() {}
