#include "component.h"
#include "DrawStrategy.h"
#include <stdio.h>

void IComponent::draw(const GraphicsContext &gc)
{
	if (!(alive && visible)) return;
}

/**
 * IComponent version handles MSG_FOCUS, MSG_UNFOCUS and MSG_KILL.
 * Any other message is passed to onHandleMessage.
 */
void IComponent::handleMessage(int msg)
{
	switch (msg)
	{
	case MSG_FOCUS:
		awake = true;
		visible = true;
		onFocus();
		break;
	case MSG_UNFOCUS:
		awake = false;
		visible = false;
		break;
	case MSG_KILL:
		kill();
		break;
	default:
		bool handled = onHandleMessage(msg);
		if (!handled)
		{
			pushMsg(msg); // let parent handle it.
		}
		break;
	}

}
