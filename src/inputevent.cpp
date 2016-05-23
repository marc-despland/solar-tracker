#include "inputevent.h"

InputEvent::InputEvent(int index, int state):Event("Phidget Input Event") {
	this->index=index;
	this->state=state;
}

int InputEvent::getIndex() {
	return this->index;
}

int InputEvent::getState() {
	return this->state;
}