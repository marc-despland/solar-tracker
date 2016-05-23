#include "tracker.h"
#include "phidget.h"
#include "earth.h"
#include <unistd.h>
#include "log.h"
Tracker * Tracker::me=new Tracker();


Tracker * Tracker::tracker() {
	return me;
}

void Tracker::attachPhidget() {
	if (Config::config()->hasPhidget()) {
		Phidget::singleton->addInputHandler(Tracker::me);
	}
}

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
		usleep(200000);
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


void Tracker::inputEvent(int index, int state) {
	if (index==Config::config()->inputIndexScan()) {
		if (state) {
			Phidget * captor=Phidget::singleton;
			if (Config::config()->outputIndexScan()!=Config::NOTDEFINED) {
				Log::logger->log("TRACKER",NOTICE) << "Light the LED " << Config::config()->outputIndexScan()<< endl;
				captor->setOutput(Config::config()->outputIndexScan(),1);
			}
			this->scan();
			if (Config::config()->outputIndexScan()!=Config::NOTDEFINED) {
				Log::logger->log("TRACKER",NOTICE) << "Unlight the LED " << Config::config()->outputIndexScan()<< endl;
				captor->setOutput(Config::config()->outputIndexScan(),0);
			}
		}
	}

}