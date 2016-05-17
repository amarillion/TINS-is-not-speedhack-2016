/*
 * componentBuilder.h
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#ifndef COMPONENTBUILDER_H_
#define COMPONENTBUILDER_H_

#include <memory>
#include "motion.h"

class IContainer;
class IComponent;

struct ALLEGRO_FONT;

class ComponentBuilder {
public:
	ComponentBuilder(std::shared_ptr<IComponent> aComponent);

	ComponentBuilder &motion(IMotionPtr motion);
	ComponentBuilder &linear(int x, int y);

	std::shared_ptr<IComponent> get();
	ComponentBuilder & center(std::shared_ptr<IContainer> ic);
	ComponentBuilder & xy(double x, double y);
	ComponentBuilder & xywh(double x, double y, double w, double h);
	ComponentBuilder & font(ALLEGRO_FONT *font);
//	ComponentBuilder & steps(std::shared_ptr<Steps> steps);
private:
	std::shared_ptr<IComponent> component;
};

#endif /* COMPONENTBUILDER_H_ */
