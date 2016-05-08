#include "servo.h"
#include "maestro.h"
#include "log.h"

std::map<int , Servo *>	Servo::servos;


Servo::Servo(uint8_t servo, std::string name) {
	this->num=servo;
	this->min=Maestro::getServoMinPosition(servo);
	this->max=Maestro::getServoMaxPosition(servo);
	this->position=6000;
	Maestro::setTarget(this->num, this->position);
	this->name=name;
	Servo::servos[this->num]=this;
	Log::logger->log("SERVO",DEBUG) << "Servo " << servo << " : " << name << " created"<< endl;
		
}

void Servo::setPosition(uint16_t position) {
	if (position>this->max) position=max;
	if (position<this->min) position=min;
	this->position=position;
	Maestro::setTarget(this->num, this->position);
}

void Servo::setSpeed(uint16_t speed) {
	Maestro::setSpeed(this->num, speed);
}
void Servo::setAcceleration(uint8_t acceleration) {
	Maestro::setAcceleration(this->num, acceleration);
}

uint16_t Servo::getPosition() {
	return(this->position);
}

void Servo::setAngle(double angle) {
	if (angle>180) angle=180;

	this->setPosition(((angle-90)*40)+6000);
}

double Servo::getAngle() {
	return((((double) (this->getPosition()-6000))/40)+90);
}

std::string Servo::getName() {
	return this->name;
}

uint16_t Servo::getMin() {
	return this->min;
}

uint16_t Servo::getMax() {
	return this->max;
}

void Servo::setMin(uint16_t value) {
	Maestro::setServoMinPosition(this->num,value);
	this->min=Maestro::getServoMinPosition(this->num);
}

void Servo::setMax(uint16_t value) {
	Maestro::setServoMaxPosition(this->num,value);
	this->max=Maestro::getServoMaxPosition(this->num);
}

void Servo::setNeutral(uint16_t value) {
	Maestro::setServoNeutralPosition(this->num,value);
	//this->neutral=Maestro::getServoNeutralPosition(this->num);
}