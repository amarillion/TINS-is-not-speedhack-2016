/*
 * CutScene.cpp
 *
 *  Created on: 5 Aug 2012
 *      Author: martijn
 */

#include "CutScene.h"
#include "color.h"
#include <allegro5/allegro.h>
#include "anim.h"
#include <memory>
#include "ComponentBuilder.h"

using namespace std;

CutScene::CutScene(int exitCode) : buttonSkip(ALLEGRO_KEY_ESCAPE), exitCode(exitCode)
{
}

void CutScene::addFrame (IComponentPtr comp)
{
	sequence.push_back (comp);
	comp->setFont(sfont);
}

IContainerPtr CutScene::newPage ()
{
	IContainerPtr scene = make_shared<IContainer>();
	addFrame(scene);
	scene->add(ClearScreen::build(BLACK).get());
	return scene;
}


void CutScene::onFocus()
{
	current = sequence.begin();
}

void CutScene::draw(const GraphicsContext &gc)
{
	if (current != sequence.end())
		(*current)->draw(gc);
}

void CutScene::update()
{
	IComponent::update();

	if (current != sequence.end())
	{
		int result = 0;

		(*current)->update();
		while ((*current)->hasMsg())
		{
			// any value at all
			result = (*current)->popMsg();
		}

		if (result != 0)
		{
			current++;
			if (current == sequence.end())
			{
				pushMsg(exitCode);
			}
		}

	}

	if (buttonSkip.justPressed())
	{
		pushMsg(exitCode);
		return; // skip animation
	}

	return;
}

