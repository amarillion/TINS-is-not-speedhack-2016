#include "menubase.h"
#include <allegro5/allegro.h>
#include "color.h"
#include <assert.h>
#include "sound.h"
#include <stdio.h>
#include <allegro5/allegro_font.h>
#include "util.h"

using namespace std;

void MenuItem::draw(const GraphicsContext &gc)
{
	assert (parent);

	ALLEGRO_COLOR color = getColor();

	al_set_target_bitmap (gc.buffer);
	al_draw_text (parent->sfont, color, getw() / 2, y, ALLEGRO_ALIGN_CENTER, getText().c_str());
}

ALLEGRO_COLOR MenuItem::getColor()
{
	ALLEGRO_COLOR color = parent->colorNormal;
	if (!enabled) color = parent->colorDisabled;
	if (flashing)
	{
		if ((parent->tFlash % 10) < 5) color = parent->colorFlash2;
	}
	else if (isSelected() && (parent->tFlash % 30) < 15) color = parent->colorFlash1;
	return color;
}

bool MenuItem::isSelected()
{
	return (parent->getSelectedItem().get() == this);
}

void ActionMenuItem::handleEvent(ALLEGRO_EVENT &event)
{
	if (event.type != ALLEGRO_EVENT_KEY_CHAR) return; // not interested in any others...

	switch (event.keyboard.keycode)
	{
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
			pushMsg(MenuItem::MENU_PREV);
			break;
		case ALLEGRO_KEY_DOWN:
		case ALLEGRO_KEY_RIGHT:
			pushMsg(MenuItem::MENU_NEXT);
			break;
		case ALLEGRO_KEY_ENTER:
		case ALLEGRO_KEY_SPACE:
			pushMsg(action);
			break;
	}
}

void ToggleMenuItem::handleEvent(ALLEGRO_EVENT &event)
{
	if (event.type != ALLEGRO_EVENT_KEY_CHAR) return; // not interested in any others...

	switch (event.keyboard.keycode)
	{
		case ALLEGRO_KEY_LEFT:
		case ALLEGRO_KEY_UP:
			pushMsg(MenuItem::MENU_PREV);
			break;
		case ALLEGRO_KEY_DOWN:
		case ALLEGRO_KEY_RIGHT:
			pushMsg(MenuItem::MENU_NEXT);
			break;
		case ALLEGRO_KEY_ENTER:
		case ALLEGRO_KEY_SPACE:
			toggle = !toggle;
			pushMsg(action);
			break;
	}
}

ToggleMenuItem::ToggleMenuItem(int _action, std::string _a, std::string _b, std::string _hint) :
		action(_action), a(_a), b(_b), hint(_hint), toggle(false) {}

void MenuList::calculateLayout(int left, int top, int right, int bottom)
{
	std::vector<MenuItemPtr>::iterator i;
	int count = 0;

	for (i = items.begin(); i != items.end(); ++i)
		if ((*i)->isVisible()) count++;

	int y = top;
	int dy = (bottom - top) / (count + 2);
	for (i = items.begin(); i != items.end(); ++i)
	{
		if ((*i)->isVisible())
		{
			(*i)->setLocation(left, y, right - left, dy);
			y += dy;
		}
	}

	hint->setDimension(right - left, dy);
}

void MenuList::onFocus()
{
	selected = 0;
	tFlash = 0;
	calculateLayout(0, topMargin, w, h - bottomMargin);
	hint->sety (h - bottomMargin);
}

void MenuList::onUpdate()
{
	tFlash++;
}

void MenuList::handleEvent(ALLEGRO_EVENT &event)
{
	items[selected]->handleEvent(event);

	while (items[selected]->hasMsg())
	{
		int result = 0;
		int action = items[selected]->popMsg();
		switch (action)
		{
		case MenuItem::MENU_NEXT:
			next();
			break;
		case MenuItem::MENU_PREV:
			prev();
			break;
		case MenuItem::MENU_NONE:
			// do nothing.
			break;
		default:
			// let parent class handle action
			if (sound_enter && sound) sound->playSample (sound_enter);
			pushMsg(action);
		}
	}
}

void Hint::draw(const GraphicsContext &gc)
{
	MenuItemPtr item = parent->getSelectedItem();
	assert (sfont);
	string hint = item->getHint();
	al_set_target_bitmap (gc.buffer);
	al_draw_text (sfont, WHITE, getw() / 2,
		y, ALLEGRO_ALIGN_CENTER, hint.c_str());
}

void MenuList::prev()
{
	do
	{
		if (selected == 0) selected = items.size() - 1;
		else selected--;
	} while (!(items[selected]->isEnabled() &&
			items[selected]->isVisible()));
	tFlash = 0;
	if (sound_cursor && sound) sound->playSample (sound_cursor);
}

MenuList::MenuList(Sound *_sound)
{
	sound = _sound;
	sound_enter = NULL; sound_cursor = NULL;
	awake = false;
	visible = false;

	topMargin = 60;
	bottomMargin = 100;

	colorNormal = YELLOW;
	colorFlash1 = RED;
	colorFlash2 = GREEN;
	colorDisabled = GREY;

	hint = make_shared<Hint>(this);
	add (hint);
}

void MenuList::next()
{
	do
	{
		if (++selected >= items.size()) selected = 0;
	} while (!(items[selected]->isEnabled() &&
			items[selected]->isVisible()));
	tFlash = 0;
	if (sound_cursor && sound) sound->playSample (sound_cursor);
}

MenuBuilder::MenuBuilder(IContainer *parent, Sound *sound)
{
	result = make_shared<MenuList>(sound);
	parent->add(result, IContainer::FLAG_SLEEP);
	assert (parent->getFont());
	assert (result->getFont());
}
