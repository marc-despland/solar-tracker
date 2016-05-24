#include "regulator.h"

#include "phidget.h"
#include "earth.h"
#include <unistd.h>
#include "log.h"
Regulator * Regulator::me=new Regulator();


Regulator * Regulator::regulator() {
	return me;
}

void Regulator::attachPhidget() {
	if (Config::config()->hasPhidget()) {
		Phidget::singleton->addInputHandler(Regulator::me);
	}
}

Regulator::Regulator() {

}


void Regulator::inputEvent(int index, int state) {
	if (index==Config::config()->inputIndexScan()) {
		if (state) {
			Phidget * captor=Phidget::singleton;
			if (Config::config()->outputIndexEarth()!=Config::NOTDEFINED) {
				Log::logger->log("TRACKER",NOTICE) << "Light the LED " << Config::config()->outputIndexEarth()<< endl;
				captor->setOutput(Config::config()->outputIndexEarth(),1);
			}
			this->scan();
			if (Config::config()->outputIndexEarth()!=Config::NOTDEFINED) {
				Log::logger->log("TRACKER",NOTICE) << "Unlight the LED " << Config::config()->outputIndexEarth()<< endl;
				captor->setOutput(Config::config()->outputIndexEarth(),0);
			}
		}
	}
}

void Regulator::scan() {
	Earth * earth=Earth::instance();
	Phidget * captor=Phidget::singleton;
	bool reguled=false;
	uint16_t start=earth->getMin();
	uint16_t end=earth->getMax();
	uint16_t step=(end-start)/100;
	uint16_t position=earth->getPosition();
	while (!reguled) {
		double left=captor->getLeftLux();
		double right=captor->getRightLux();
		reguled=(left>=right*0.95) && (left<=right*1.05);
		if (!reguled && (left>right*1.05)) {
			position+=step;
			if (position>end) {
				reguled=true; 
			} else {
				earth->setPosition(position);
				usleep(100000);
			}
		}
		if (!reguled && (left<right*0.95)) {
			position-=step;
			if (position>end) {
				reguled=true; 
			} else {
				earth->setPosition(position);
				usleep(100000);
			}
		}
		Log::logger->log("REGULATOR",NOTICE) << "Reguled Left="<<left<<" - Right="<<right  << endl;
	}
}