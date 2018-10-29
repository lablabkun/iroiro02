#include "opencv01.h"

void Mouse::callback(int eventType, int x, int y, int flags, void* userdata)
{
	mouseparam *ptr = static_cast<mouseparam*> (userdata);
	ptr->x = x;
	ptr->y = y;
	ptr->event = eventType;
	ptr->flags = flags;
}