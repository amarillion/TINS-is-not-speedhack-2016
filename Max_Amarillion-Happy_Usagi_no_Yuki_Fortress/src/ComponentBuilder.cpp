/*
 * componentBuilder.cpp
 */

#include "ComponentBuilder.h"
#include "component.h"
#include "motionimpl.h"
#include <allegro5/allegro.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include "util.h"
#include "IContainer.h"

using namespace std;

ComponentBuilder::ComponentBuilder(shared_ptr<IComponent> aComponent) : component(aComponent) { }

ComponentBuilder &ComponentBuilder::motion(IMotionPtr motion)
{
	component->setMotion(motion);
	return *this;
}

ComponentBuilder &ComponentBuilder::linear(int x, int y)
{
	IMotionPtr temp = IMotionPtr(new Linear (x, y));
	component->setMotion(temp);
	return *this;
}

ComponentBuilder & ComponentBuilder::center(IContainerPtr parent)
{
	assert (parent != NULL);
	component->setx(parent->getw() / 2);
	component->sety(parent->geth() / 2);
	return *this;
}

ComponentBuilder & ComponentBuilder::xy(double x, double y)
{
	component->setx(x);
	component->sety(y);
	return *this;
}

ComponentBuilder & ComponentBuilder::xywh(double x, double y, double w, double h)
{
	component->setx(x);
	component->sety(y);
	component->setDimension(w, h);
	return *this;
}

ComponentBuilder & ComponentBuilder::font(ALLEGRO_FONT *font)
{
	component->setFont(font);
	return *this;
}

IComponentPtr ComponentBuilder::get()
{
	return component;
}

