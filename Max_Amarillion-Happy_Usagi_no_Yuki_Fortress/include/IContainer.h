#ifndef ICONTAINER_H_
#define ICONTAINER_H_

#include "DrawStrategy.h"
#include <list>
#include "component.h"

class IContainer : public IComponent
{
protected:
	std::list<IComponentPtr> children;
	IComponentPtr focus;
public:
	enum {
		/** use add (..., FLAG_SLEEP) to add an element without giving it focus, to be awoken later. */
		FLAG_SLEEP = 1,
		/** use add (..., FLAG_BOTTOM) for it to be drawn as background */
		FLAG_BOTTOM = 2 };
	IContainer();
	virtual void draw(const GraphicsContext &gc) override;
	virtual void update() override;
	virtual void handleEvent (ALLEGRO_EVENT &evt) override;
	virtual void setFont(ALLEGRO_FONT *font);
	void setTimer(int msec, int event);
	virtual void purge();
	virtual void killAll();
	virtual void add (IComponentPtr item, int flags = 0);
	void setFocus(IComponentPtr _focus);
};

typedef std::shared_ptr<IContainer> IContainerPtr;

#endif /* ICONTAINER_H_ */
