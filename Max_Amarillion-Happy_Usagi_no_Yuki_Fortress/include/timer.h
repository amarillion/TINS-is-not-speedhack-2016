#ifndef TIMER_H
#define TIMER_H

#include "component.h"
#include <memory>

class ITimer
{
public:
	virtual int getCounter() = 0;
};

class Timer : public IComponent
{
	int msg;
	int maxCounter;
public:
	Timer(int maxCounter, int msg) :
		msg(msg), maxCounter(maxCounter)
		{ setAwake(true); setVisible(false); }
	virtual ~Timer();
	virtual void update() override;
};

typedef std::shared_ptr<Timer> TimerPtr;

#endif
