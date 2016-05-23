#ifndef _EVENTLISTENER_H
#define _EVENTLISTENER_H

#include "event.h"

class EventListener {
public:
	virtual void received(Event * event)=0;

};
#endif