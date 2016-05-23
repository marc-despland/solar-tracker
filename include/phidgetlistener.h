#ifndef _PHIDGETLISTENER_H
#define _PHIDGETLISTENER_H

#include "eventlistener.h"

class PhidgetListener : public EventListener {
public:
	virtual void inputEvent(int index, int state)=0;
	void received(Event * event);
};

#endif