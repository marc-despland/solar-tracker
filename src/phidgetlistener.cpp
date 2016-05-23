#include "phidgetlistener.h"
#include "inputevent.h"

void PhidgetListener::received(Event * event) {
	InputEvent * evt= (InputEvent *) event;
	this->inputEvent(evt->getIndex(), evt->getState());
}