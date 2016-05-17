/*
 * CutScene.h
 *
 *  Created on: 5 Aug 2012
 *      Author: martijn
 */

#ifndef CUTSCENE_H_
#define CUTSCENE_H_

#include "component.h"
#include <vector>
#include "IContainer.h"
#include "input.h"

class CutScene : public IComponent
{
private:
	Input buttonSkip;
	int exitCode;
	std::vector<IComponentPtr> sequence;
	std::vector<IComponentPtr>::iterator current;
	void addFrame (IComponentPtr comp);
public:
	IContainerPtr newPage ();
	CutScene(int exitCode);
	virtual void onFocus() override;
	virtual void draw(const GraphicsContext &gc) override;
	virtual void update() override;
};

#endif /* CUTSCENE_H_ */
