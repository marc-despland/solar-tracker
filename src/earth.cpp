#include "earth.h"
#include <thread>
#include <unistd.h>
#include "log.h"
#include "config.h"
#include "phidget.h"

Earth * Earth::singleton=NULL;

void Earth::init(uint8_t servo, double longitude, double latitude) {
	Earth::singleton=new Earth(servo, longitude, latitude);
}

bool Earth::running() {
	return Earth::singleton->go;
}
double Earth::getRealAngle() {
	return Earth::singleton->getAngle();
}

double Earth::getTheoricalAngle() {
	return Earth::getTheoricalAngle(Earth::singleton->longitude);
}


void Earth::setReverseServo(Servo * reverse) {
	this->reverse=reverse;
	this->reverse->setPosition(((this->reverse->getMax()-this->reverse->getMin())/2)-(((this->getMax()-this->getMin())/2)-this->getPosition())/2);
}

Earth * Earth::instance() {
	return Earth::singleton;
}

double Earth::getTheoricalAngle(double longitude) {
	time_t now;
	struct tm * nowinfo;
	time(&(now));
	nowinfo = localtime (&now);

	double hour=((double) nowinfo->tm_sec)/6000;
	hour+=((double) nowinfo->tm_min)/60;
	hour+=nowinfo->tm_hour;
	Log::logger->log("Earth",DEBUG) << "It's " << hour << "h localtime" << endl;
	if (nowinfo->tm_isdst>0) {
		hour-=2;
	} else {
		hour-=1;
	}
	Log::logger->log("Earth",DEBUG) << "It's " << hour << "h solar time" << endl;
	hour-=longitude/15;
	Log::logger->log("Earth",DEBUG) << "It's " << hour << "h solar time after longitude correction ("<<(longitude/15)<< ")" << endl;
	double angle=90+(hour-12)*15;
	Log::logger->log("Earth",DEBUG) << "Earth calculate aproximative solar angle " << angle << endl;
	return angle;
}

Earth::Earth(uint8_t servo, double longitude, double latitude) : Servo( servo, "Earth rotation") {
	time(&(this->inittime));
	this->go=false;
	this->run=NULL;
	this->longitude=longitude;
	this->latitude=latitude;
	this->setAngle(Earth::getTheoricalAngle(longitude));
	this->initpos=this->getPosition();
	this->reverse=NULL;
	Log::logger->log("Earth",DEBUG) << "Earth tracker initialize servo("<<(uint16_t) servo<< ") position:" << this->initpos << endl;
}

void Earth::start() {
	Earth::singleton->initpos=Earth::singleton->getPosition();
	time(&(Earth::singleton->inittime));
	Earth::singleton->go=true;
	Log::logger->log("Earth",DEBUG) << "Earth tracker starting at position:" << Earth::singleton->initpos << endl;
	Earth::singleton->run= new std::thread(Earth::execute);
}

void Earth::stop() {
	Earth::singleton->go=false;
	Log::logger->log("Earth",DEBUG) << "Earth tracker stoping" << endl;
	Earth::singleton->run->join();
}


void Earth::execute() {
	time_t now;
	while (Earth::singleton->go) {
		sleep(6);
		time(&now);
		int elapse=difftime(now, Earth::singleton->inittime);
		Earth::singleton->setPosition((elapse/6)+Earth::singleton->initpos);
		Log::logger->log("Earth",DEBUG) << "Earth tracker running at position:" << Earth::singleton->getPosition() << " after " << elapse <<"s"<< endl;
	}

}


void Earth::setAngle(double angle) {
	if (this->reverse!=NULL) {
		double oldearth=this->getAngle();
		double oldreverse=this->reverse->getAngle();
		this->reverse->setAngle(oldreverse-(angle-oldearth)/2);
	}
	this->Servo::setAngle(angle);
}
void Earth::setPosition(uint16_t position) {
	if (this->reverse!=NULL) {
		uint16_t oldearth=this->getPosition();
		uint16_t oldreverse=this->reverse->getPosition();
		this->reverse->setPosition(oldreverse-(position-oldearth)/2);
	}
	this->Servo::setPosition(position);
}

