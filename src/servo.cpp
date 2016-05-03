#include "servo.h"
#include "maestro.h"


Servo::Servo() {
	this->num=0;
	this->min=2400;
	this->max=9600;
	this->position=6000;
}

Servo::Servo(uint8_t servo) : Servo() {
	this->num=servo;
	Maestro::controller->setTarget(this->num, this->position);
}

void Servo::setPosition(uint16_t position) {
	this->position=position;
	Maestro::controller->setTarget(this->num, this->position);
}

uint16_t Servo::getPosition() {
	return(this->position);
}

void Servo::setAngle(double angle) {
	if (angle>180) angle=180;
	int a=(int) angle;
	this->setPosition(((a-90)*40)+6000);
}

uint16_t Servo::getAngle() {
	return(((this->getPosition()-6000)/40)+90);
}