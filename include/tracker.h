#ifndef _TRACKER_H
#define _TRACKER_H
#include "config.h"
#include "phidgetaction.h"

class Tracker : public PhidgetAction{
public:
	static Tracker * tracker();
	static void attachPhidget();
	void scan();
	void inputEvent(int index, int state);
	double angleleft;
	double angleright;
	double maxleft;
	double maxright;
	double maxlux;
	double anglelux;

protected:
	static Tracker * me;
	Tracker();

};

#endif