#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "config.h"
#include "phidgetlistener.h"
#include "tracker.h"
#include "regulator.h"
#include "phidget.h"
#include "log.h"

class Controller : public PhidgetListener {
public:
	static Controller * controller();
	static void 		attachPhidget();
	void 				init();
	void 				inputEvent(int index, int state);
	TrackerSearchResult * 			scan();
	RegulatorSearchResult * 		regulate();

protected:
	static Controller * me;
						Controller();
	Tracker * 			tracker;
	Regulator * 		regulator;
	Earth * 			earth;
	Servo * 			reverse;
};

#endif