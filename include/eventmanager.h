#ifndef _EVENTMANAGER_H
#define _EVENTMANAGER_H
#include <vector>
#include "eventlistener.h"

class EventManager {
public:
	EventManager();
protected:
	void sendEvent(Event * event);
	void addListener(EventListener * listener);
	std::vector<EventListener *> listeners;
	static void sentToListener(EventListener * listener, Event * event);
};
#endif