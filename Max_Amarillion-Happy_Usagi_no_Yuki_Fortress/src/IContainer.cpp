/*
 * IContainer.cpp
 *
 *  Created on: 3 Aug 2012
 *      Author: martijn
 */

#include "IContainer.h"
#include "timer.h"
#include <allegro5/allegro.h>
#include <algorithm>

using namespace std;

IContainer::IContainer() { }

void IContainer::setFont(ALLEGRO_FONT *font)
{
	// set the font and the font of children, if needed.
	IComponent::setFont(font);
	list<IComponentPtr>::iterator i;
	for (i = children.begin(); i != children.end(); ++i)
	{
		if (!((*i)->getFont()))
			(*i)->setFont(font);
	}
}

void IContainer::add (std::shared_ptr<IComponent> item, int flags)
{
	if (flags & FLAG_BOTTOM)
		children.push_front(item);
	else
		children.push_back (item);
	item->setAwake(!(flags & FLAG_SLEEP));
	item->setVisible(!(flags & FLAG_SLEEP));

	if (item->getw() <= 0 && item->geth() <= 0)
	{
		item->setDimension(getw(), geth());
	}

	//TODO: might needs some rethinking.
	// Inheritance of font like this is not ideal.
	// it is not clear when to set them if they should be different from parent.

	if (!(item->getFont()))
		item->setFont(sfont);
}

void IContainer::update()
{
	IComponent::update();
	int result = 0;

	for (auto i = children.begin(); i != children.end(); ++i)
	{
		if ((*i)->isAwake())
		{
			(*i)->update();

			while ((*i)->hasMsg())
			{
				handleMessage((*i)->popMsg());
			}
		}
	}

	purge();
}

void IContainer::draw(const GraphicsContext &gc)
{
	list<std::shared_ptr<IComponent> >::iterator i;
	for (i = children.begin(); i != children.end(); ++i)
	{
		if ((*i)->isVisible())
			(*i)->draw(gc);
	}
}

void IContainer::setFocus(IComponentPtr _focus)
{
	assert (_focus);
	assert (find(children.begin(), children.end(), _focus) != children.end()); // trying to set focus to somethign that wasn't added first

	if (focus)
	{
		focus->handleMessage(MSG_UNFOCUS);
	}
	focus = _focus;
	focus->handleMessage(MSG_FOCUS);
}

void IContainer::handleEvent (ALLEGRO_EVENT &event)
{
	switch (event.type)
	{
		// mouse events

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		case ALLEGRO_EVENT_MOUSE_AXES:
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			ALLEGRO_MOUSE_STATE mouse_state;
			al_get_mouse_state(&mouse_state);
			int current_mouse_x = event.mouse.x;
			int current_mouse_y = event.mouse.y;

			// we have to send a click message
			// go through in reverse drawing order and find the component on top.
			for (auto i = children.rbegin(); i != children.rend(); i++)
			{
				if ((*i)->contains(current_mouse_x, current_mouse_y) && (*i)->isVisible())
				{
					(*i)->handleEvent(event);
					while ((*i)->hasMsg())
					{
						int msg = (*i)->popMsg();
						handleMessage(msg);
					}

					//TODO - item should request focus itself...
//					if ((*i)->wantsFocus())
//					   setFocus (*i);

					break; // break from loop
				}
			}
//			if (!focus) searchNextFocusItem();
		}

		// non-mouse events
		default:
			if (focus)
			{
				focus->handleEvent(event);

				while (focus->hasMsg())
				{
					int msg = focus->popMsg();
					handleMessage(msg);
				}
			}
			break;
	}

}

void IContainer::setTimer(int msec, int event)
{
	TimerPtr timer = TimerPtr(new Timer(msec, event));
	add (timer);
}

class MyComponentRemover
{
public:
	bool operator()(IComponentPtr o)
	{
		if (!o->isAlive())
		{
			o.reset();
			return 1;
		}
		return 0;
	}
};

void IContainer::purge()
{
	// remove all that are not alive!
	children.remove_if (MyComponentRemover());
}

void IContainer::killAll()
{
	list<IComponentPtr>::iterator i;
	for (i = children.begin(); i != children.end(); ++i)
	{
		(*i)->kill();
	}
	purge();
}
