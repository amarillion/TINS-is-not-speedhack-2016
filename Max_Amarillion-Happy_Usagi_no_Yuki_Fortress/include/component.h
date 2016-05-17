#ifndef COMPONENT_H
#define COMPONENT_H

#include "graphicscontext.h"
#include <list>
#include "motion.h"
#include <memory>
#include "motionimpl.h"
#include <allegro5/allegro.h>
#include <iostream>

struct ALLEGRO_FONT;

class IComponent : public std::enable_shared_from_this<IComponent>
{
private:
	// messages destined for the parent.
	std::list<int> msgQ;

protected:
	// if awake is set to false, update() will not be called.
	bool awake;
	// if alive is set to false, this component will be removed
	// neither update() nor draw() will be called.
	bool alive;
	// if visible is set to false, draw() will not be called
	bool visible;

	ALLEGRO_FONT *sfont;

	//TODO: perhaps the Motion should get the counter and not the component.
	int counter;

	IMotionPtr motion;

	double x, y;
	int w, h;

	/**
	 * Add a message to the queue. The message queue will be read by this component's parent.
	 * pushMsg() may be called from any method including handleEvent, handleMessage and update.
	 */
	void pushMsg(int msg) { msgQ.push_back(msg); assert (msgQ.size() < 10); }
public:
	/*
	 * Check if there are any messages queued.
	 * Should be called by this component's parent.
	 */
	bool hasMsg() { return !msgQ.empty(); }

	/*
	 * Pop a message from the queue
	 * Should be called by this component's parent.
	 */
	int popMsg() { int msg = msgQ.front(); msgQ.pop_front(); return msg; }


	enum { MSG_FOCUS = 2000, MSG_UNFOCUS, MSG_KILL };

	IComponent() : IComponent(0, 0, 0, 0) { }
	virtual ~IComponent() {}

	IComponent (int newx, int newy, int neww, int newh) : awake(true), alive(true), visible(true), sfont(NULL), counter(0), motion(), x(newx), y(newy), w(neww), h(newh)
	{}


	/**
	 * update, called for each heartbeat tick.
	 *
	 * Don't override this, override onUpdate, unless you want to change behavior drastically
	 */
	virtual void update() { counter++; onUpdate(); }

	/**
	 * update, called for each heartbeat tick.
	 * Designed to be overridden.
	 */
	virtual void onUpdate() {}

	virtual void draw(const GraphicsContext &gc);

	void setMotion(const IMotionPtr &value) { motion = value; }
	IMotionPtr getMotion() { return motion; }

	int getCounter() { return counter; }

	/**
	 * handle a control message.
	 * Override onHandleMessage, unless you wish to alter behavior drastically
	 */
	virtual void handleMessage(int msg);

	/**
	 * handle Control message. destined for overriding.
	 *
	 * return true if the message has been handled, otherwise,
	 * the message will be passed on to the parent of this component.
	 */
	virtual bool onHandleMessage(int msg) { return false; };

	/**
	 * Called whenever this component receives a MSG_FOCUS message.
	 * By default, component will become awake and visible.
	 *
	 * Override this to do additional initialisation when focus is gained.
	 */
	virtual void onFocus() { }

	/**
	 * Override this,
	 *
	 * 	returns true in case this element can receive focus, and will be picked when saerching for the next
	 * 	focus item after pressing tab.
	 *
	 * 	returns false in case this component can not receive focus, will be skipped when searching for next
	 * 	focus item after pressing tab.
	 *
	 */
    virtual bool wantsFocus () { return false; }

	/** handle input event. */
	virtual void handleEvent(ALLEGRO_EVENT &event) {}

	// if awake is set to false, update() will not be called.
	bool isAwake() { return awake; }
	void setAwake(bool value) { awake = value; }

	// if alive is set to false, this component will be removed
	// neither update() nor draw() will be called.
	bool isAlive() { return alive; }
	void kill() { alive = false; }

	// if visible is set to false, draw() will not be called
	bool isVisible() { return visible; }
	void setVisible(bool value) { visible = value; }

	virtual void setFont(ALLEGRO_FONT *font) { this->sfont = font; }
	ALLEGRO_FONT *getFont() { return sfont; }

	double gety() { return y; }
	double getx() { return x; }
	int getw () const { return w; }
	int geth () const { return h; }
	void sety(double _y) { y = _y; }
	void setx(double _x) { x = _x; }

	void setxy(double _x, double _y) { x = _x; y = _y; }
	void setLocation (double _x, double _y, int _w, int _h) { x = _x; y = _y; w = _w; h = _h; }
	void setDimension (int _w, int _h) { w = _w; h = _h; }

	/**
	check if the point cx, cy is within the bounding box of the gui item.
	usefull for checking if the mouse is over the gui item.
	*/
	bool contains (int cx, int cy) {
	    return (cx >= x && cy >= y && cx < (x + w) && cy < (y + h));
	}
};

typedef std::shared_ptr<IComponent> IComponentPtr;

#endif
