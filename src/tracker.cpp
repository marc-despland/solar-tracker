#include "tracker.h"
#include "phidget.h"
#include "earth.h"
#include <unistd.h>

Tracker::Tracker() {
	this->angleleft=-1;
	this->angleright=-1;
	this->maxleft=-1;
	this->maxright=-1;
	this->anglelux=-1;
	this->maxlux=-1;

}

void Tracker::scan() {
	Earth * earth=Earth::instance();
	Phidget * captor=Phidget::singleton;
	uint16_t start=earth->getMin();
	uint16_t end=earth->getMax();
	uint16_t step=(end-start)/100;
	earth->setPosition(start);
	usleep(1000000);
	for (uint16_t i=start; i<end; i+=step) {
		earth->setPosition(i);
		usleep(50000);
		if (this->maxleft<captor->getLeftLux()) {
			this->maxleft=captor->getLeftLux();
			this->angleleft=earth->getAngle();
		}
		if (this->maxright<captor->getRightLux()) {
			this->maxright=captor->getRightLux();
			this->angleright=earth->getAngle();
		}
		if (this->maxlux<(captor->getRightLux()+captor->getLeftLux())) {
			this->maxlux=(captor->getRightLux()+captor->getLeftLux());
			this->anglelux=earth->getAngle();
		}
	}
	earth->setAngle(this->anglelux);

}
