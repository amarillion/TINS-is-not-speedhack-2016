#ifndef STEPS_H
#define STEPS_H

#include "component.h"
#include <functional>

class StepsBuilder;

class Steps
{
private:
	std::list<std::function<void()> actions;
public:
//	static StepsBuilder build();
	void add(std::function<void()> action) { actions.push_back(action); }
};

/*
class StepsBuilder
{
	StepsBuilder &setState(int state);
	StepsBuilder &linear(int time, float dx, float dy);
	StepsBuilder &wait(int time);
	std::shared_ptr<Steps> get();
};
*/

#endif
