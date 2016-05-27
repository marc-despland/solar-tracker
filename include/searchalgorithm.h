#ifndef _SEARCHALGORITHM_H
#define _SEARCHALGORITHM_H
#include "phidget.h"
#include "earth.h"

class SearchResult {
public:

};

class SearchAlgorithm {
public:
	SearchAlgorithm();
	void setEarthServo(Earth * earth);
	void setPhidgetCaptor(Phidget * phidget);
	virtual SearchResult * search()=0;
protected:
	Earth * earth;
	Phidget * captor;

};
#endif