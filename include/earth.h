#ifndef _EARTH_H
#define _EARTH_H

#include "servo.h"
#include <time.h>
#include <thread>
#include "phidgetlistener.h"

class Earth : public Servo, PhidgetListener {

public: 
	
	static void init(uint8_t servo, double longitude, double latitude);

	static void start();
	static void stop();
	static double getTheoricalAngle();
	static double getTheoricalAngle(double longitude);
	static double getRealAngle();
	static bool running();
	static Earth * instance();
	void inputEvent(int index, int state);
	static void attachPhidget();
protected:
	static Earth * singleton;
	Earth(uint8_t servo, double longitude, double latitude);
	uint16_t initpos;
	time_t inittime;
	double longitude;
	double latitude;
	bool go;
	std::thread * run;
	static void execute();
	


};


#endif