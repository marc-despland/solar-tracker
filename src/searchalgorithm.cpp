#include "searchalgorithm.h"



SearchAlgorithm::SearchAlgorithm() {
	this->earth=NULL;
	this->captor=NULL;
}


/*SearchAlgorithm::~SearchAlgorithm() {
}*/


void SearchAlgorithm::setEarthServo(Earth * earth) {
	this->earth=earth;
}
void SearchAlgorithm::setPhidgetCaptor(Phidget * phidget) {
	this->captor=phidget;
}