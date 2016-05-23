#include "eventmanager.h"
#include <thread>
#include <unistd.h>
#include "log.h"


EventManager::EventManager() {

}


void EventManager::sendEvent(Event * event) {
	for (std::vector<EventListener *>::iterator it = this->listeners.begin() ; it != this->listeners.end(); ++it) {
		std::thread (EventManager::sentToListener,(*it), event).detach();
	}
}

void EventManager::addListener(EventListener * listener) {
	this->listeners.push_back(listener);
}


void EventManager::sentToListener(EventListener * listener, Event * event) {
	listener->received(event);
}
