#ifndef _INPUTEVENT_H
#define _INPUTEVENT_H
#include "event.h"


class InputEvent: public Event {
public:
		InputEvent(int index, int state);
		int getState();
		int getIndex();
protected:
		int index;
		int state;

};
#endif