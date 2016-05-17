#ifndef GAME_H
#define GAME_H

#include "component.h"
#include "IContainer.h"

class Engine;

class Game : public IContainer
{
public:
	virtual void init() = 0;

	static std::shared_ptr<Game> newInstance(Engine *parent);
};

#endif
