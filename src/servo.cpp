#include "servo.h"
#include "maestro.h"


std::map<int , Servo *>	Servo::servos;


Servo::Servo(uint8_t servo, std::string name) {
	this->num=servo;
	Maestro::controller->setTarget(this->num, this->position);
	this->min=2400;
	this->max=9600;
	this->position=6000;
	this->name=name;
	Servo::servos[this->num]=this;
}

void Servo::setPosition(uint16_t position) {
	if (position>this->max) position=max;
	if (position<this->min) position=min;
	this->position=position;
	Maestro::controller->setTarget(this->num, this->position);
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

