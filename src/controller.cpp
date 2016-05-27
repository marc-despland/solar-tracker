#include "controller.h"

Controller * Controller::me=new Controller();


Controller * Controller::controller() {
	return me;
}

void Controller::attachPhidget() {
	if (Config::config()->hasPhidget()) {
		Phidget::singleton->addInputHandler(Controller::me);
		Controller::me->tracker->setPhidgetCaptor(Phidget::singleton);
		Controller::me->regulator->setPhidgetCaptor(Phidget::singleton);
	}
}

TrackerSearchResult * Controller::scan() {
	return (TrackerSearchResult *) this->tracker->search();
}

RegulatorSearchResult * Controller::regulate() {
	return (RegulatorSearchResult *) this->regulator->search();
}

Controller::Controller() {
	this->tracker=new Tracker();
	this->regulator=new Regulator();
	this->earth=NULL;
	this->reverse=NULL;
}


void Controller::init() {
	if (Config::config()->hasMaestro()) {
		if (Maestro::init()) {
			if (Config::config()->hasServoEarth()) {
				Earth::init(Config::config()->servoEarth(),Config::config()->longitude(),Config::config()->latitude());
				this->earth=Earth::instance();
				this->tracker->setEarthServo(this->earth);
				this->regulator->setEarthServo(this->earth);
				if (this->reverse!=NULL) this->earth->setReverseServo(this->reverse);
			}
            if (Config::config()->hasServoReverseEarth()) {
            	this->reverse=new Servo(Config::config()->servoReverseEarth(), "reverse");
            	if (this->earth!=NULL) this->earth->setReverseServo(this->reverse);
            }
        } else {
            Log::logger->log("CONTROLLER",EMERGENCY) << "No Maestro controller detected" << endl;
        }
	}
}


void Controller::inputEvent(int index, int state) {
	if (index==Config::config()->inputIndexScan()) {
		if (state) {
			Phidget * captor=Phidget::singleton;
			if (Config::config()->outputIndexScan()!=Config::NOTDEFINED) {
				captor->setOutput(Config::config()->outputIndexScan(),1);
			}
			this->tracker->search();
			if (Config::config()->outputIndexScan()!=Config::NOTDEFINED) {
				captor->setOutput(Config::config()->outputIndexScan(),0);
			}
		}
	}
	if (index==Config::config()->inputIndexRegulator()) {
		if (state) {
			Phidget * captor=Phidget::singleton;
			if (Config::config()->outputIndexRegulator()!=Config::NOTDEFINED) {
				captor->setOutput(Config::config()->outputIndexRegulator(),1);
			}
			this->regulator->search();
			if (Config::config()->outputIndexRegulator()!=Config::NOTDEFINED) {
				captor->setOutput(Config::config()->outputIndexRegulator(),0);
			}
		}
	}

}